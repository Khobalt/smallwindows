#include <windows.h>
#include <vector>
#include <cstdio>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

char const *szClassName = "DrawingApp";

// Structure to store drawing points
struct DrawPoint {
    int x, y;
    COLORREF color;
    bool isStart;
};

// Structure for undo system
struct UndoState {
    std::vector<DrawPoint> points;
};

// Global variables for drawing
std::vector<DrawPoint> drawingPoints;
std::vector<UndoState> undoStack;
std::vector<UndoState> redoStack;
bool isDrawing = false;
COLORREF currentColor = RGB(0, 0, 0); // Start with black
int brushSize = 2; // Default brush size

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
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
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    // Register the window class.
    if (!RegisterClassEx(&wincl))
        return 0;

    // Create the window.
    hwnd = CreateWindowEx(
        0,
        szClassName,
        "Drawing App - Click and drag to draw, R/G/B for colors, C to clear",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    // Display the window on the screen.
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop: Retrieve and dispatch messages until a WM_QUIT message is received.
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            // Save state for undo before starting new stroke
            UndoState state;
            state.points = drawingPoints;
            undoStack.push_back(state);
            redoStack.clear(); // Clear redo stack when new action is performed
            
            // Start drawing
            isDrawing = true;
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            DrawPoint point = {x, y, currentColor, true};
            drawingPoints.push_back(point);
            SetCapture(hwnd); // Capture mouse even outside window
        }
        break;

        case WM_MOUSEMOVE:
        {
            if (isDrawing && (wParam & MK_LBUTTON))
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                DrawPoint point = {x, y, currentColor, false};
                drawingPoints.push_back(point);
                
                // Force immediate redraw of just the new line
                HDC hdc = GetDC(hwnd);
                HPEN pen = CreatePen(PS_SOLID, brushSize, currentColor);
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
        break;

        case WM_LBUTTONUP:
        {
            // Stop drawing
            isDrawing = false;
            ReleaseCapture();
        }
        break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9':
                    brushSize = wParam - '0'; // Convert char to number
                    break;
                case 'Z': // Undo (Ctrl+Z handled separately)
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        if (!undoStack.empty()) {
                            // Save current state to redo stack
                            UndoState redoState;
                            redoState.points = drawingPoints;
                            redoStack.push_back(redoState);
                            
                            // Restore previous state
                            drawingPoints = undoStack.back().points;
                            undoStack.pop_back();
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                    }
                    break;
                case 'Y': // Redo (Ctrl+Y)
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        if (!redoStack.empty()) {
                            // Save current state to undo stack
                            UndoState undoState;
                            undoState.points = drawingPoints;
                            undoStack.push_back(undoState);
                            
                            // Restore redo state
                            drawingPoints = redoStack.back().points;
                            redoStack.pop_back();
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                    }
                    break;
                case 'R': // Red color
                    currentColor = RGB(255, 0, 0);
                    break;
                case 'G': // Green color
                    currentColor = RGB(0, 255, 0);
                    break;
                case 'B': // Blue color
                    currentColor = RGB(0, 0, 255);
                    break;
                case 'K': // Black color
                    currentColor = RGB(0, 0, 0);
                    break;
                case 'W': // White color
                    currentColor = RGB(255, 255, 255);
                    break;
                case 'L': // Yellow color (L for Light)
                    currentColor = RGB(255, 255, 0);
                    break;
                case 'M': // Magenta color
                    currentColor = RGB(255, 0, 255);
                    break;
                case 'A': // Cyan color
                    currentColor = RGB(0, 255, 255);
                    break;
                case 'O': // Orange color
                    currentColor = RGB(255, 165, 0);
                    break;
                case 'P': // Purple color
                    currentColor = RGB(128, 0, 128);
                    break;
                case 'C': // Clear canvas
                    if (!(GetKeyState(VK_CONTROL) & 0x8000)) {
                        // Save state for undo
                        UndoState state;
                        state.points = drawingPoints;
                        undoStack.push_back(state);
                        redoStack.clear();
                        
                        drawingPoints.clear();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;
                case VK_ESCAPE: // Exit on Escape
                    PostQuitMessage(0);
                    break;
            }
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Clear background
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

            // Draw all the stored points
            if (!drawingPoints.empty()) {
                HPEN pen = NULL;
                COLORREF lastColor = drawingPoints[0].color;
                pen = CreatePen(PS_SOLID, brushSize, lastColor);
                HPEN oldPen = (HPEN)SelectObject(hdc, pen);

                for (size_t i = 0; i < drawingPoints.size(); i++) {
                    DrawPoint& point = drawingPoints[i];
                    
                    // Change pen color if needed
                    if (point.color != lastColor) {
                        SelectObject(hdc, oldPen);
                        DeleteObject(pen);
                        pen = CreatePen(PS_SOLID, brushSize, point.color);
                        oldPen = (HPEN)SelectObject(hdc, pen);
                        lastColor = point.color;
                    }

                    if (point.isStart || i == 0) {
                        MoveToEx(hdc, point.x, point.y, NULL);
                    } else {
                        LineTo(hdc, point.x, point.y);
                    }
                }

                SelectObject(hdc, oldPen);
                DeleteObject(pen);
            }

            // Draw instructions
            SetTextColor(hdc, RGB(100, 100, 100));
            TextOutA(hdc, 10, 10, "PaintStudio Pro v1.1 - Professional Drawing App", 46);
            TextOutA(hdc, 10, 30, "1-9: Brush size | R/G/B/K/W/L/M/A/O/P: Colors", 45);
            TextOutA(hdc, 10, 50, "C: Clear | Ctrl+Z: Undo | Ctrl+Y: Redo | ESC: Exit", 50);
            
            // Draw current brush size and color indicator
            SetTextColor(hdc, RGB(0, 0, 0));
            char statusText[100];
            sprintf(statusText, "Brush: %d", brushSize);
            TextOutA(hdc, 10, 70, statusText, strlen(statusText));
            
            // Color indicator rectangle
            HBRUSH colorBrush = CreateSolidBrush(currentColor);
            RECT colorRect = {120, 70, 140, 90};
            FillRect(hdc, &colorRect, colorBrush);
            DeleteObject(colorBrush);
            
            // Black border around color indicator
            HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            HPEN oldBorderPen = (HPEN)SelectObject(hdc, borderPen);
            Rectangle(hdc, 120, 70, 140, 90);
            SelectObject(hdc, oldBorderPen);
            DeleteObject(borderPen);

            EndPaint(hwnd, &ps);
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
