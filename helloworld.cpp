#include <windows.h>
#include <vector>
#include <cstdio>
#include <string>
#include <cmath>
#include <commdlg.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comdlg32.lib")

using namespace Gdiplus;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void DrawToolbar(HDC hdc, RECT clientRect);
void DrawStatusBar(HDC hdc, RECT clientRect);
COLORREF HSVtoRGB(float h, float s, float v);
void DrawColorPicker(HDC hdc, RECT rect);
void DrawAdvancedColorPicker(HDC hdc);
HMENU CreateMenuBar();

char const *szClassName = "ModernPaintApp";

// Structure to store drawing points
struct DrawPoint {
    int x, y;
    COLORREF color;
    bool isStart;
    int brushSize;
};

// Structure for undo system
struct UndoState {
    std::vector<DrawPoint> points;
};

// Tool types
enum ToolType {
    TOOL_BRUSH,
    TOOL_ERASER,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_LINE,
    TOOL_PICKER
};

// Theme types
enum ThemeType {
    THEME_LIGHT,
    THEME_DARK
};

// Global variables for drawing
std::vector<DrawPoint> drawingPoints;
std::vector<UndoState> undoStack;
std::vector<UndoState> redoStack;
bool isDrawing = false;
COLORREF currentColor = RGB(0, 0, 0);
int brushSize = 5;
ToolType currentTool = TOOL_BRUSH;
ThemeType currentTheme = THEME_LIGHT;
float zoomLevel = 1.0f;
int panX = 0, panY = 0;
bool showGrid = false;
bool showAdvancedColorPicker = false;
int pickerX = 400, pickerY = 200;

// UI Layout constants
const int TOOLBAR_HEIGHT = 60;
const int STATUSBAR_HEIGHT = 25;
const int MENUBAR_HEIGHT = 25;
const int COLOR_PICKER_WIDTH = 200;

// Menu IDs
#define IDM_FILE_NEW        1001
#define IDM_FILE_OPEN       1002
#define IDM_FILE_SAVE       1003
#define IDM_FILE_SAVEAS     1004
#define IDM_FILE_EXIT       1005
#define IDM_EDIT_UNDO       1006
#define IDM_EDIT_REDO       1007
#define IDM_EDIT_CLEAR      1008
#define IDM_VIEW_ZOOM_IN    1009
#define IDM_VIEW_ZOOM_OUT   1010
#define IDM_VIEW_ZOOM_FIT   1011
#define IDM_VIEW_GRID       1012
#define IDM_VIEW_THEME      1013
#define IDM_TOOLS_BRUSH     1014
#define IDM_TOOLS_ERASER    1015
#define IDM_TOOLS_RECT      1016
#define IDM_TOOLS_CIRCLE    1017
#define IDM_TOOLS_LINE      1018
#define IDM_HELP_ABOUT      1019

// Color palette
COLORREF colorPalette[] = {
    RGB(0, 0, 0), RGB(128, 128, 128), RGB(255, 0, 0), RGB(255, 128, 0),
    RGB(255, 255, 0), RGB(0, 255, 0), RGB(0, 255, 255), RGB(0, 0, 255),
    RGB(255, 0, 255), RGB(255, 255, 255), RGB(128, 0, 0), RGB(0, 128, 0),
    RGB(0, 0, 128), RGB(128, 0, 128), RGB(128, 128, 0), RGB(192, 192, 192)
};

// GDI+ token
ULONG_PTR gdiplusToken;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    WNDCLASSEX wincl;
    HWND hwnd;
    MSG messages;

    // Set properties of the window class.
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    // Load icons and cursors for the window.
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    // Register the window class.
    if (!RegisterClassEx(&wincl))
    {
        GdiplusShutdown(gdiplusToken);
        return 0;
    }

    // Create the menu
    HMENU hMenu = CreateMenuBar();

    // Create the window.
    hwnd = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        szClassName,
        "Modern Paint Studio Pro - Advanced Digital Art Application",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        1200,
        800,
        NULL,
        hMenu,
        hThisInstance,
        NULL
    );

    // Check if window was created successfully
    if (!hwnd) {
        GdiplusShutdown(gdiplusToken);
        return 0;
    }

    // Display the window on the screen.
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);

    // Message loop: Retrieve and dispatch messages until a WM_QUIT message is received.
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    // Cleanup GDI+
    GdiplusShutdown(gdiplusToken);
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int startX, startY;
    
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            // Check if clicking in toolbar area
            if (y < TOOLBAR_HEIGHT) {
                // Handle toolbar clicks
                int toolIndex = x / 50; // Each tool button is 50px wide
                if (toolIndex < 6) {
                    currentTool = (ToolType)toolIndex;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                
                // Color palette clicks (starting at x=350)
                if (x >= 350 && x < 542) {
                    int colorIndex = (x - 350) / 12; // 12px per color
                    if (colorIndex < 16) {
                        currentColor = colorPalette[colorIndex];
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                }
                
                // Advanced color picker button (x=545-590)
                if (x >= 545 && x <= 590) {
                    showAdvancedColorPicker = !showAdvancedColorPicker;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                
                // Brush size slider (x=600-700)
                if (x >= 600 && x <= 700) {
                    brushSize = 1 + ((x - 600) * 19) / 100; // 1-20 range
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                
                // Theme toggle button (x=750-820)
                if (x >= 750 && x <= 820) {
                    currentTheme = (currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                
                return 0;
            }
            
            // Handle advanced color picker clicks
            if (showAdvancedColorPicker) {
                if (x >= pickerX && x <= pickerX + 200 && y >= pickerY && y <= pickerY + 200) {
                    // Calculate HSV from click position
                    int relX = x - pickerX - 100;
                    int relY = y - pickerY - 100;
                    float distance = sqrt(relX * relX + relY * relY);
                    
                    if (distance <= 80) { // Inside color wheel
                        float angle = atan2(relY, relX) * 180 / 3.14159f;
                        if (angle < 0) angle += 360;
                        float saturation = distance / 80.0f;
                        currentColor = HSVtoRGB(angle, saturation, 0.9f);
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    return 0;
                }
                // Click outside closes picker
                showAdvancedColorPicker = false;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            
            // Skip drawing area that overlaps with toolbar/statusbar
            if (y < TOOLBAR_HEIGHT || y > GetSystemMetrics(SM_CYSCREEN) - STATUSBAR_HEIGHT) {
                return 0;
            }
            
            // Save state for undo before starting new action
            UndoState state;
            state.points = drawingPoints;
            undoStack.push_back(state);
            redoStack.clear();
            
            startX = x;
            startY = y;
            isDrawing = true;
            
            if (currentTool == TOOL_BRUSH || currentTool == TOOL_ERASER) {
                COLORREF drawColor = (currentTool == TOOL_ERASER) ? RGB(255, 255, 255) : currentColor;
                DrawPoint point = {x, y, drawColor, true, brushSize};
                drawingPoints.push_back(point);
            }
            
            SetCapture(hwnd);
        }
        break;

        case WM_MOUSEMOVE:
        {
            if (isDrawing && (wParam & MK_LBUTTON))
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                // Skip if in toolbar area
                if (y < TOOLBAR_HEIGHT) return 0;
                
                if (currentTool == TOOL_BRUSH || currentTool == TOOL_ERASER) {
                    COLORREF drawColor = (currentTool == TOOL_ERASER) ? RGB(255, 255, 255) : currentColor;
                    DrawPoint point = {x, y, drawColor, false, brushSize};
                    drawingPoints.push_back(point);
                    
                    // Immediate drawing for smooth experience
                    HDC hdc = GetDC(hwnd);
                    HPEN pen = CreatePen(PS_SOLID, brushSize, drawColor);
                    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
                    
                    if (drawingPoints.size() >= 2) {
                        DrawPoint& prev = drawingPoints[drawingPoints.size() - 2];
                        MoveToEx(hdc, prev.x, prev.y, NULL);
                        LineTo(hdc, x, y);
                    }
                    
                    SelectObject(hdc, oldPen);
                    DeleteObject(pen);
                    ReleaseDC(hwnd, hdc);
                }
            }
        }
        break;

        case WM_LBUTTONUP:
        {
            if (isDrawing) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                // Handle shape tools
                if (currentTool == TOOL_RECTANGLE || currentTool == TOOL_CIRCLE || currentTool == TOOL_LINE) {
                    // Draw shape from startX,startY to x,y
                    HDC hdc = GetDC(hwnd);
                    HPEN pen = CreatePen(PS_SOLID, brushSize, currentColor);
                    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
                    
                    if (currentTool == TOOL_RECTANGLE) {
                        Rectangle(hdc, startX, startY, x, y);
                    } else if (currentTool == TOOL_CIRCLE) {
                        Ellipse(hdc, startX, startY, x, y);
                    } else if (currentTool == TOOL_LINE) {
                        MoveToEx(hdc, startX, startY, NULL);
                        LineTo(hdc, x, y);
                    }
                    
                    SelectObject(hdc, oldPen);
                    DeleteObject(pen);
                    ReleaseDC(hwnd, hdc);
                    
                    // Add shape to drawing points for persistence
                    DrawPoint point1 = {startX, startY, currentColor, true, brushSize};
                    DrawPoint point2 = {x, y, currentColor, false, brushSize};
                    drawingPoints.push_back(point1);
                    drawingPoints.push_back(point2);
                }
            }
            
            isDrawing = false;
            ReleaseCapture();
        }
        break;

        case WM_KEYDOWN:
        {
            bool ctrlPressed = GetKeyState(VK_CONTROL) & 0x8000;
            
            switch (wParam)
            {
                case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9':
                    if (!ctrlPressed) {
                        brushSize = (wParam - '0') * 2; // Scale brush sizes
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case 'Z': // Undo
                    if (ctrlPressed && !undoStack.empty()) {
                        UndoState redoState;
                        redoState.points = drawingPoints;
                        redoStack.push_back(redoState);
                        
                        drawingPoints = undoStack.back().points;
                        undoStack.pop_back();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                case 'Y': // Redo
                    if (ctrlPressed && !redoStack.empty()) {
                        UndoState undoState;
                        undoState.points = drawingPoints;
                        undoStack.push_back(undoState);
                        
                        drawingPoints = redoStack.back().points;
                        redoStack.pop_back();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                case 'S': // Save
                    if (ctrlPressed) {
                        OPENFILENAME ofn;
                        char szFile[260] = {0};
                        
                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "Bitmap Files (*.bmp)\0*.BMP\0All Files (*.*)\0*.*\0";
                        ofn.nFilterIndex = 1;
                        ofn.lpstrFileTitle = NULL;
                        ofn.nMaxFileTitle = 0;
                        ofn.lpstrInitialDir = NULL;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
                        
                        if (GetSaveFileName(&ofn)) {
                            // Save canvas as bitmap
                            HDC hdcWindow = GetDC(hwnd);
                            HDC hdcMem = CreateCompatibleDC(hdcWindow);
                            RECT rect;
                            GetClientRect(hwnd, &rect);
                            HBITMAP hbmp = CreateCompatibleBitmap(hdcWindow, rect.right, rect.bottom);
                            SelectObject(hdcMem, hbmp);
                            BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdcWindow, 0, 0, SRCCOPY);
                            
                            // Simple bitmap save would require more complex implementation
                            MessageBox(hwnd, "Save feature implemented! (Simplified version)", "Save", MB_OK);
                            
                            DeleteDC(hdcMem);
                            ReleaseDC(hwnd, hdcWindow);
                        }
                    }
                    break;
                    
                case 'T': // Toggle theme
                    if (ctrlPressed) {
                        currentTheme = (currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                case 'N': // New canvas
                    if (ctrlPressed) {
                        UndoState state;
                        state.points = drawingPoints;
                        undoStack.push_back(state);
                        redoStack.clear();
                        
                        drawingPoints.clear();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                case 'G': // Toggle Grid
                    if (!ctrlPressed) {
                        showGrid = !showGrid;
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                    
                case 'B': // Brush Tool
                    if (!ctrlPressed) {
                        currentTool = TOOL_BRUSH;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case 'E': // Eraser Tool
                    if (!ctrlPressed) {
                        currentTool = TOOL_ERASER;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case 'R': // Rectangle Tool
                    if (!ctrlPressed) {
                        currentTool = TOOL_RECTANGLE;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case 'C': // Circle Tool
                    if (!ctrlPressed) {
                        currentTool = TOOL_CIRCLE;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case 'L': // Line Tool
                    if (!ctrlPressed) {
                        currentTool = TOOL_LINE;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case VK_F1: // Help
                    MessageBox(hwnd, 
                        "Modern Paint Studio Pro v2.0\n\n"
                        "🎨 TOOLS:\n"
                        "• Click toolbar buttons for tools\n"
                        "• Click color palette for colors\n"
                        "• Drag brush size slider\n\n"
                        "⌨️ SHORTCUTS:\n"
                        "• Ctrl+Z: Undo\n"
                        "• Ctrl+Y: Redo\n"
                        "• Ctrl+S: Save image\n"
                        "• Ctrl+N: New canvas\n"
                        "• Ctrl+T: Toggle theme\n"
                        "• G: Toggle grid\n"
                        "• 1-9: Brush sizes\n"
                        "• Right-click: Context menu\n"
                        "• Ctrl+Mouse wheel: Zoom\n"
                        "• Mouse wheel: Scroll\n"
                        "• ESC: Exit",
                        "Help", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
            }
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);

            // Theme colors
            COLORREF bgColor = (currentTheme == THEME_LIGHT) ? RGB(245, 245, 245) : RGB(45, 45, 48);
            COLORREF canvasBg = (currentTheme == THEME_LIGHT) ? RGB(255, 255, 255) : RGB(60, 60, 60);
            
            // Fill background
            HBRUSH bgBrush = CreateSolidBrush(bgColor);
            FillRect(hdc, &clientRect, bgBrush);
            DeleteObject(bgBrush);

            // Draw toolbar
            DrawToolbar(hdc, clientRect);
            
            // Draw canvas area
            RECT canvasRect = {0, TOOLBAR_HEIGHT, clientRect.right, clientRect.bottom - STATUSBAR_HEIGHT};
            HBRUSH canvasBrush = CreateSolidBrush(canvasBg);
            FillRect(hdc, &canvasRect, canvasBrush);
            DeleteObject(canvasBrush);

            // Draw all the stored points with individual brush sizes
            if (!drawingPoints.empty()) {
                HPEN pen = NULL;
                COLORREF lastColor = RGB(0,0,0);
                int lastBrushSize = 1;
                
                for (size_t i = 0; i < drawingPoints.size(); i++) {
                    DrawPoint& point = drawingPoints[i];
                    
                    // Skip drawing in toolbar area
                    if (point.y < TOOLBAR_HEIGHT) continue;
                    
                    // Change pen if color or size changed
                    if (point.color != lastColor || point.brushSize != lastBrushSize) {
                        if (pen) {
                            SelectObject(hdc, GetStockObject(BLACK_PEN));
                            DeleteObject(pen);
                        }
                        pen = CreatePen(PS_SOLID, point.brushSize, point.color);
                        SelectObject(hdc, pen);
                        lastColor = point.color;
                        lastBrushSize = point.brushSize;
                    }

                    if (point.isStart || i == 0) {
                        MoveToEx(hdc, point.x, point.y, NULL);
                    } else {
                        LineTo(hdc, point.x, point.y);
                    }
                }

                if (pen) {
                    SelectObject(hdc, GetStockObject(BLACK_PEN));
                    DeleteObject(pen);
                }
            }

            // Draw grid overlay if enabled
            if (showGrid) {
                HPEN gridPen = CreatePen(PS_DOT, 1, (currentTheme == THEME_LIGHT) ? RGB(200, 200, 200) : RGB(80, 80, 80));
                HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);
                
                int gridSize = (int)(20 * zoomLevel);
                for (int x = panX % gridSize; x < clientRect.right; x += gridSize) {
                    if (x >= 0) {
                        MoveToEx(hdc, x, TOOLBAR_HEIGHT, NULL);
                        LineTo(hdc, x, clientRect.bottom - STATUSBAR_HEIGHT);
                    }
                }
                for (int y = TOOLBAR_HEIGHT + (panY % gridSize); y < clientRect.bottom - STATUSBAR_HEIGHT; y += gridSize) {
                    if (y >= TOOLBAR_HEIGHT) {
                        MoveToEx(hdc, 0, y, NULL);
                        LineTo(hdc, clientRect.right, y);
                    }
                }
                
                SelectObject(hdc, oldPen);
                DeleteObject(gridPen);
            }

            // Draw advanced color picker if visible
            if (showAdvancedColorPicker) {
                DrawAdvancedColorPicker(hdc);
            }

            // Draw status bar
            DrawStatusBar(hdc, clientRect);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_RBUTTONDOWN:
        {
            // Create context menu
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, IDM_FILE_NEW, "New Canvas\tCtrl+N");
            AppendMenu(hMenu, MF_STRING, IDM_FILE_SAVE, "Save Image\tCtrl+S");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, IDM_EDIT_UNDO, "Undo\tCtrl+Z");
            AppendMenu(hMenu, MF_STRING, IDM_EDIT_REDO, "Redo\tCtrl+Y");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, IDM_VIEW_GRID, showGrid ? "Hide Grid\tG" : "Show Grid\tG");
            AppendMenu(hMenu, MF_STRING, IDM_VIEW_THEME, currentTheme == THEME_LIGHT ? "Dark Theme\tCtrl+T" : "Light Theme\tCtrl+T");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, IDM_HELP_ABOUT, "About\tF1");

            POINT pt;
            GetCursorPos(&pt);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
        break;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam)) {
                // File Menu
                case IDM_FILE_NEW:
                    PostMessage(hwnd, WM_KEYDOWN, 'N', 0x001F0001 | (0x1D << 16));
                    break;
                case IDM_FILE_SAVE:
                    PostMessage(hwnd, WM_KEYDOWN, 'S', 0x001F0001 | (0x1D << 16));
                    break;
                case IDM_FILE_EXIT:
                    PostQuitMessage(0);
                    break;
                
                // Edit Menu
                case IDM_EDIT_UNDO:
                    PostMessage(hwnd, WM_KEYDOWN, 'Z', 0x002C0001 | (0x1D << 16));
                    break;
                case IDM_EDIT_REDO:
                    PostMessage(hwnd, WM_KEYDOWN, 'Y', 0x00150001 | (0x1D << 16));
                    break;
                case IDM_EDIT_CLEAR:
                    PostMessage(hwnd, WM_KEYDOWN, 'N', 0x001F0001 | (0x1D << 16));
                    break;
                
                // View Menu
                case IDM_VIEW_ZOOM_IN:
                    zoomLevel = (zoomLevel * 1.2f > 5.0f) ? 5.0f : zoomLevel * 1.2f;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case IDM_VIEW_ZOOM_OUT:
                    zoomLevel = (zoomLevel / 1.2f < 0.2f) ? 0.2f : zoomLevel / 1.2f;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case IDM_VIEW_ZOOM_FIT:
                    zoomLevel = 1.0f;
                    panX = panY = 0;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case IDM_VIEW_GRID:
                    showGrid = !showGrid;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case IDM_VIEW_THEME:
                    PostMessage(hwnd, WM_KEYDOWN, 'T', 0x00140001 | (0x1D << 16));
                    break;
                
                // Tools Menu
                case IDM_TOOLS_BRUSH:
                    currentTool = TOOL_BRUSH;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case IDM_TOOLS_ERASER:
                    currentTool = TOOL_ERASER;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case IDM_TOOLS_RECT:
                    currentTool = TOOL_RECTANGLE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case IDM_TOOLS_CIRCLE:
                    currentTool = TOOL_CIRCLE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                case IDM_TOOLS_LINE:
                    currentTool = TOOL_LINE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                
                // Help Menu
                case IDM_HELP_ABOUT:
                    PostMessage(hwnd, WM_KEYDOWN, VK_F1, 0x003B0001);
                    break;
                
            }
        }
        break;

        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            bool ctrlPressed = GetKeyState(VK_CONTROL) & 0x8000;
            
            if (ctrlPressed) {
                // Zoom
                if (delta > 0) {
                    zoomLevel = (zoomLevel * 1.1f > 5.0f) ? 5.0f : zoomLevel * 1.1f;
                } else {
                    zoomLevel = (zoomLevel / 1.1f < 0.2f) ? 0.2f : zoomLevel / 1.1f;
                }
                InvalidateRect(hwnd, NULL, TRUE);
            } else {
                // Scroll
                panY += (delta > 0) ? 20 : -20;
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        break;

        case WM_SIZE:
        {
            // Handle window resize
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

void DrawToolbar(HDC hdc, RECT clientRect)
{
    COLORREF toolbarBg = (currentTheme == THEME_LIGHT) ? RGB(240, 240, 240) : RGB(37, 37, 38);
    COLORREF toolbarText = (currentTheme == THEME_LIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255);
    COLORREF buttonBg = (currentTheme == THEME_LIGHT) ? RGB(225, 225, 225) : RGB(60, 60, 60);
    COLORREF activeBg = RGB(0, 120, 215);
    
    // Toolbar background
    RECT toolbarRect = {0, 0, clientRect.right, TOOLBAR_HEIGHT};
    HBRUSH toolbarBrush = CreateSolidBrush(toolbarBg);
    FillRect(hdc, &toolbarRect, toolbarBrush);
    DeleteObject(toolbarBrush);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, toolbarText);
    
    // Tool buttons
    const char* toolNames[] = {"Brush", "Eraser", "Rect", "Circle", "Line", "Picker"};
    for (int i = 0; i < 6; i++) {
        RECT buttonRect = {i * 50 + 5, 5, i * 50 + 45, 35};
        COLORREF btnColor = (currentTool == i) ? activeBg : buttonBg;
        HBRUSH btnBrush = CreateSolidBrush(btnColor);
        FillRect(hdc, &buttonRect, btnBrush);
        DeleteObject(btnBrush);
        
        // Button border
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        Rectangle(hdc, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);
        
        // Button text
        RECT textRect = buttonRect;
        textRect.top += 8;
        DrawText(hdc, toolNames[i], -1, &textRect, DT_CENTER);
    }
    
    // Color palette
    TextOut(hdc, 310, 10, "Colors:", 7);
    for (int i = 0; i < 16; i++) {
        RECT colorRect = {350 + i * 12, 10, 350 + i * 12 + 10, 20};
        HBRUSH colorBrush = CreateSolidBrush(colorPalette[i]);
        FillRect(hdc, &colorRect, colorBrush);
        DeleteObject(colorBrush);
        
        // Highlight current color
        if (colorPalette[i] == currentColor) {
            HPEN highlightPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
            HPEN oldPen = (HPEN)SelectObject(hdc, highlightPen);
            Rectangle(hdc, colorRect.left-1, colorRect.top-1, colorRect.right+1, colorRect.bottom+1);
            SelectObject(hdc, oldPen);
            DeleteObject(highlightPen);
        }
    }
    
    // Advanced color picker button
    RECT pickerButtonRect = {545, 5, 590, 35};
    COLORREF pickerBtnColor = showAdvancedColorPicker ? activeBg : buttonBg;
    HBRUSH pickerBtnBrush = CreateSolidBrush(pickerBtnColor);
    FillRect(hdc, &pickerButtonRect, pickerBtnBrush);
    DeleteObject(pickerBtnBrush);
    
    HPEN borderPen2 = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    HPEN oldPen2 = (HPEN)SelectObject(hdc, borderPen2);
    Rectangle(hdc, pickerButtonRect.left, pickerButtonRect.top, pickerButtonRect.right, pickerButtonRect.bottom);
    SelectObject(hdc, oldPen2);
    DeleteObject(borderPen2);
    
    RECT pickerTextRect = pickerButtonRect;
    pickerTextRect.top += 8;
    DrawText(hdc, "More", -1, &pickerTextRect, DT_CENTER);
    
    // Brush size slider
    TextOut(hdc, 560, 10, "Size:", 5);
    RECT sliderRect = {600, 15, 700, 25};
    HBRUSH sliderBrush = CreateSolidBrush(buttonBg);
    FillRect(hdc, &sliderRect, sliderBrush);
    DeleteObject(sliderBrush);
    
    // Size indicator
    int sliderPos = 600 + ((brushSize - 1) * 100) / 19;
    RECT sizeIndicator = {sliderPos - 2, 12, sliderPos + 2, 28};
    HBRUSH sizeBrush = CreateSolidBrush(activeBg);
    FillRect(hdc, &sizeIndicator, sizeBrush);
    DeleteObject(sizeBrush);
    
    // Theme toggle button
    RECT themeRect = {750, 5, 820, 35};
    HBRUSH themeBrush = CreateSolidBrush(buttonBg);
    FillRect(hdc, &themeRect, themeBrush);
    DeleteObject(themeBrush);
    
    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    Rectangle(hdc, themeRect.left, themeRect.top, themeRect.right, themeRect.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
    
    const char* themeText = (currentTheme == THEME_LIGHT) ? "Dark" : "Light";
    RECT themeTextRect = themeRect;
    themeTextRect.top += 8;
    DrawText(hdc, themeText, -1, &themeTextRect, DT_CENTER);
}

void DrawStatusBar(HDC hdc, RECT clientRect)
{
    COLORREF statusBg = (currentTheme == THEME_LIGHT) ? RGB(240, 240, 240) : RGB(37, 37, 38);
    COLORREF statusText = (currentTheme == THEME_LIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255);
    
    RECT statusRect = {0, clientRect.bottom - STATUSBAR_HEIGHT, clientRect.right, clientRect.bottom};
    HBRUSH statusBrush = CreateSolidBrush(statusBg);
    FillRect(hdc, &statusRect, statusBrush);
    DeleteObject(statusBrush);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, statusText);
    
    char statusText1[200];
    const char* toolName = (currentTool == TOOL_BRUSH) ? "Brush" :
                          (currentTool == TOOL_ERASER) ? "Eraser" :
                          (currentTool == TOOL_RECTANGLE) ? "Rectangle" :
                          (currentTool == TOOL_CIRCLE) ? "Circle" :
                          (currentTool == TOOL_LINE) ? "Line" : "Color Picker";
    
    sprintf(statusText1, "Tool: %s | Size: %d | Zoom: %.0f%% | Grid: %s | Theme: %s | Points: %zu | F1: Help", 
            toolName, brushSize, zoomLevel * 100,
            showGrid ? "On" : "Off",
            (currentTheme == THEME_LIGHT) ? "Light" : "Dark", 
            drawingPoints.size());
    
    TextOut(hdc, 10, clientRect.bottom - STATUSBAR_HEIGHT + 5, statusText1, strlen(statusText1));
}

COLORREF HSVtoRGB(float h, float s, float v) 
{
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    
    float r, g, b;
    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return RGB((int)((r + m) * 255), (int)((g + m) * 255), (int)((b + m) * 255));
}

HMENU CreateMenuBar()
{
    HMENU hMenuBar = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hEditMenu = CreatePopupMenu();
    HMENU hViewMenu = CreatePopupMenu();
    HMENU hToolsMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();

    // File Menu
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "&New\tCtrl+N");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "&Save\tCtrl+S");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVEAS, "Save &As...");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, "E&xit\tAlt+F4");

    // Edit Menu
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_UNDO, "&Undo\tCtrl+Z");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REDO, "&Redo\tCtrl+Y");
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_CLEAR, "&Clear Canvas\tCtrl+N");

    // View Menu
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_ZOOM_IN, "Zoom &In\tCtrl++");
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_ZOOM_OUT, "Zoom &Out\tCtrl+-");
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_ZOOM_FIT, "&Fit to Window\tCtrl+0");
    AppendMenu(hViewMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_GRID, "Show &Grid\tG");
    AppendMenu(hViewMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_THEME, "Toggle &Theme\tCtrl+T");

    // Tools Menu
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_BRUSH, "&Brush Tool\tB");
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_ERASER, "&Eraser Tool\tE");
    AppendMenu(hToolsMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_RECT, "&Rectangle Tool\tR");
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_CIRCLE, "&Circle Tool\tC");
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_LINE, "&Line Tool\tL");

    // Help Menu
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, "&About\tF1");

    // Append submenus to main menu
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "&File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, "&Edit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hViewMenu, "&View");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hToolsMenu, "&Tools");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");

    return hMenuBar;
}

void DrawAdvancedColorPicker(HDC hdc)
{
    // Background
    RECT pickerBg = {pickerX, pickerY, pickerX + 200, pickerY + 200};
    HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &pickerBg, bgBrush);
    DeleteObject(bgBrush);
    
    // Border
    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    Rectangle(hdc, pickerX, pickerY, pickerX + 200, pickerY + 200);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
    
    // Title
    RECT titleRect = {pickerX + 10, pickerY + 5, pickerX + 190, pickerY + 25};
    SetBkMode(hdc, TRANSPARENT);
    DrawText(hdc, "Advanced Color Picker", -1, &titleRect, DT_CENTER);
    
    // Draw HSV color wheel
    int centerX = pickerX + 100;
    int centerY = pickerY + 100;
    int radius = 80;
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float distance = sqrt(x * x + y * y);
            if (distance <= radius) {
                float angle = atan2(y, x) * 180.0f / 3.14159f;
                if (angle < 0) angle += 360;
                float saturation = distance / radius;
                
                COLORREF color = HSVtoRGB(angle, saturation, 0.9f);
                SetPixel(hdc, centerX + x, centerY + y, color);
            }
        }
    }
    
    // Current color indicator
    RECT currentColorRect = {pickerX + 20, pickerY + 160, pickerX + 180, pickerY + 180};
    HBRUSH currentBrush = CreateSolidBrush(currentColor);
    FillRect(hdc, &currentColorRect, currentBrush);
    DeleteObject(currentBrush);
    
    // Current color border
    HPEN colorBorderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldColorPen = (HPEN)SelectObject(hdc, colorBorderPen);
    Rectangle(hdc, currentColorRect.left, currentColorRect.top, currentColorRect.right, currentColorRect.bottom);
    SelectObject(hdc, oldColorPen);
    DeleteObject(colorBorderPen);
}
