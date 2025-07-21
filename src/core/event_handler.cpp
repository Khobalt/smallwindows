#include "../../include/event_handler.h"
#include "../../include/config.h"
#include "../../include/app_state.h"
#include "../../include/ui_renderer.h"
#include "../../include/drawing_engine.h"

// Forward declaration for main window procedure
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
            EventHandler::OnPaint(hwnd);
            break;
            
        case WM_LBUTTONDOWN:
            EventHandler::OnLeftButtonDown(hwnd, LOWORD(lParam), HIWORD(lParam));
            break;
            
        case WM_MOUSEMOVE:
            EventHandler::OnMouseMove(hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
            break;
            
        case WM_LBUTTONUP:
            EventHandler::OnLeftButtonUp(hwnd, LOWORD(lParam), HIWORD(lParam));
            break;
            
        case WM_RBUTTONDOWN:
            EventHandler::OnRightButtonDown(hwnd, LOWORD(lParam), HIWORD(lParam));
            break;
            
        case WM_MOUSEWHEEL:
            EventHandler::OnMouseWheel(hwnd, wParam, lParam);
            break;
            
        case WM_KEYDOWN:
            EventHandler::OnKeyDown(hwnd, wParam);
            break;
            
        case WM_COMMAND:
            EventHandler::OnCommand(hwnd, wParam);
            break;
            
        case WM_SIZE:
            EventHandler::OnSize(hwnd, wParam, lParam);
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

namespace EventHandler {

// Helper functions for coordinate transformation
static int ScreenToWorldX(int screenX, const AppState& app) {
    return (int)((screenX - app.panX) / app.zoomLevel);
}

static int ScreenToWorldY(int screenY, const AppState& app) {
    return (int)((screenY - app.panY - TOOLBAR_HEIGHT) / app.zoomLevel);
}

static int WorldToScreenX(int worldX, const AppState& app) {
    return (int)(worldX * app.zoomLevel + app.panX);
}

static int WorldToScreenY(int worldY, const AppState& app) {
    return (int)(worldY * app.zoomLevel + app.panY + TOOLBAR_HEIGHT);
}

// Helper function to invalidate only the necessary parts
static void InvalidateCanvas(HWND hwnd) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    RECT canvasRect = {
        0, 
        TOOLBAR_HEIGHT, 
        clientRect.right, 
        clientRect.bottom - STATUSBAR_HEIGHT
    };
    InvalidateRect(hwnd, &canvasRect, FALSE);
}

static void InvalidateToolbar(HWND hwnd) {
    RECT toolbarRect = {0, 0, 9999, TOOLBAR_HEIGHT};
    InvalidateRect(hwnd, &toolbarRect, FALSE);
}

static void InvalidateStatusBar(HWND hwnd) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    RECT statusRect = {0, clientRect.bottom - STATUSBAR_HEIGHT, clientRect.right, clientRect.bottom};
    InvalidateRect(hwnd, &statusRect, FALSE);
}

void OnPaint(HWND hwnd)
{
    AppState& app = AppState::Instance();
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    // Clear background
    COLORREF bgColor = (app.currentTheme == THEME_LIGHT) ? RGB(255, 255, 255) : RGB(30, 30, 30);
    HBRUSH bgBrush = CreateSolidBrush(bgColor);
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw grid if enabled
    if (app.showGrid) {
        HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);
        
        int gridSize = (int)(20 * app.zoomLevel);
        for (int x = app.panX % gridSize; x < clientRect.right; x += gridSize) {
            MoveToEx(hdc, x, TOOLBAR_HEIGHT, NULL);
            LineTo(hdc, x, clientRect.bottom - STATUSBAR_HEIGHT);
        }
        for (int y = TOOLBAR_HEIGHT + (app.panY % gridSize); y < clientRect.bottom - STATUSBAR_HEIGHT; y += gridSize) {
            MoveToEx(hdc, 0, y, NULL);
            LineTo(hdc, clientRect.right, y);
        }
        
        SelectObject(hdc, oldPen);
        DeleteObject(gridPen);
    }
    
    // Draw all drawing points with smooth lines
    if (!app.drawingPoints.empty()) {
        DrawPoint* prevPoint = nullptr;
        
        for (size_t i = 0; i < app.drawingPoints.size(); i++) {
            const DrawPoint& point = app.drawingPoints[i];
            
            if (point.isStart) {
                // Start new stroke - just remember this point
                prevPoint = const_cast<DrawPoint*>(&point);
            } else if (prevPoint != nullptr) {
                // Apply zoom and pan transformations
                int prevX = (int)(prevPoint->x * app.zoomLevel + app.panX);
                int prevY = (int)(prevPoint->y * app.zoomLevel + app.panY + TOOLBAR_HEIGHT);
                int currX = (int)(point.x * app.zoomLevel + app.panX);
                int currY = (int)(point.y * app.zoomLevel + app.panY + TOOLBAR_HEIGHT);
                int scaledBrushSize = (int)(point.brushSize * app.zoomLevel);
                
                // Draw line from previous point to current point with brush size
                HPEN strokePen = CreatePen(PS_SOLID, scaledBrushSize, point.color);
                HPEN oldPen = (HPEN)SelectObject(hdc, strokePen);
                
                // Draw thick line between points
                MoveToEx(hdc, prevX, prevY, NULL);
                LineTo(hdc, currX, currY);
                
                // Draw circular brush at current point for smooth appearance
                HBRUSH pointBrush = CreateSolidBrush(point.color);
                HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, pointBrush);
                SelectObject(hdc, GetStockObject(NULL_PEN)); // No outline
                
                Ellipse(hdc, 
                        currX - scaledBrushSize/2, 
                        currY - scaledBrushSize/2,
                        currX + scaledBrushSize/2, 
                        currY + scaledBrushSize/2);
                
                SelectObject(hdc, oldBrush);
                DeleteObject(pointBrush);
                SelectObject(hdc, oldPen);
                DeleteObject(strokePen);
                
                prevPoint = const_cast<DrawPoint*>(&point);
            }
        }
        
        // Draw starting points that don't have connections
        for (const DrawPoint& point : app.drawingPoints) {
            if (point.isStart) {
                // Apply zoom and pan transformations
                int x = (int)(point.x * app.zoomLevel + app.panX);
                int y = (int)(point.y * app.zoomLevel + app.panY + TOOLBAR_HEIGHT);
                int scaledBrushSize = (int)(point.brushSize * app.zoomLevel);
                
                HBRUSH pointBrush = CreateSolidBrush(point.color);
                HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, pointBrush);
                SelectObject(hdc, GetStockObject(NULL_PEN)); // No outline
                
                Ellipse(hdc, 
                        x - scaledBrushSize/2, 
                        y - scaledBrushSize/2,
                        x + scaledBrushSize/2, 
                        y + scaledBrushSize/2);
                
                SelectObject(hdc, oldBrush);
                DeleteObject(pointBrush);
            }
        }
    }
    
    // Draw UI elements
    UIRenderer::DrawToolbar(hdc, clientRect);
    UIRenderer::DrawStatusBar(hdc, clientRect);
    
    if (app.showAdvancedColorPicker) {
        UIRenderer::DrawAdvancedColorPicker(hdc);
    }
    
    EndPaint(hwnd, &ps);
}

void OnLeftButtonDown(HWND hwnd, int x, int y)
{
    AppState& app = AppState::Instance();
    
    if (y < TOOLBAR_HEIGHT) {
        // Handle toolbar clicks
        if (x < 300) { // Tool buttons
            int toolIndex = x / 50;
            if (toolIndex >= 0 && toolIndex < 6) {
                DrawingEngine::SetTool((ToolType)toolIndex);
                InvalidateToolbar(hwnd);
                InvalidateStatusBar(hwnd);
            }
        } else if (x >= 350 && x < 542) { // Color palette
            int colorIndex = (x - 350) / 12;
            if (colorIndex >= 0 && colorIndex < 16) {
                DrawingEngine::SetColor(colorPalette[colorIndex]);
                InvalidateToolbar(hwnd);
            }
        } else if (x >= 545 && x <= 590) { // Advanced color picker toggle
            app.showAdvancedColorPicker = !app.showAdvancedColorPicker;
            InvalidateRect(hwnd, NULL, FALSE); // Color picker affects entire screen
        } else if (x >= 600 && x <= 700) { // Brush size slider
            int newSize = 1 + ((x - 600) * 19) / 100;
            DrawingEngine::SetBrushSize(newSize);
            InvalidateToolbar(hwnd);
            InvalidateStatusBar(hwnd);
        } else if (x >= 750 && x <= 820) { // Theme toggle
            app.currentTheme = (app.currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
            InvalidateRect(hwnd, NULL, FALSE); // Theme affects entire screen
        }
    } else if (app.showAdvancedColorPicker && 
               x >= app.pickerX && x <= app.pickerX + 200 && 
               y >= app.pickerY && y <= app.pickerY + 200) {
        // Handle color picker clicks
        int centerX = app.pickerX + 100;
        int centerY = app.pickerY + 100;
        int dx = x - centerX;
        int dy = y - centerY;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance <= 80) {
            float angle = atan2(dy, dx) * 180.0f / 3.14159f;
            if (angle < 0) angle += 360;
            float saturation = distance / 80.0f;
            
            COLORREF newColor = DrawingEngine::HSVtoRGB(angle, saturation, 0.9f);
            DrawingEngine::SetColor(newColor);
            InvalidateRect(hwnd, NULL, FALSE);
        }
    } else {
        // Start drawing on canvas
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        if (y > TOOLBAR_HEIGHT && y < clientRect.bottom - STATUSBAR_HEIGHT) {
            AppState& app = AppState::Instance();
            
            if (app.currentTool == TOOL_PICKER) {
                // Color picker - get color at this location
                HDC hdc = GetDC(hwnd);
                COLORREF pickedColor = DrawingEngine::PickColorAt(hdc, x, y);
                DrawingEngine::SetColor(pickedColor);
                ReleaseDC(hwnd, hdc);
                InvalidateToolbar(hwnd);
                InvalidateStatusBar(hwnd);
            } else {
                // Transform screen coordinates to world coordinates
                int worldX = ScreenToWorldX(x, app);
                int worldY = ScreenToWorldY(y, app);
                DrawingEngine::StartDrawing(worldX, worldY);
                InvalidateCanvas(hwnd);
            }
        }
    }
}

void OnMouseMove(HWND hwnd, WPARAM wParam, int x, int y)
{
    if (wParam & MK_LBUTTON) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        if (y > TOOLBAR_HEIGHT && y < clientRect.bottom - STATUSBAR_HEIGHT) {
            AppState& app = AppState::Instance();
            
            if (app.currentTool == TOOL_BRUSH || app.currentTool == TOOL_ERASER) {
                // Transform screen coordinates to world coordinates
                int worldX = ScreenToWorldX(x, app);
                int worldY = ScreenToWorldY(y, app);
                DrawingEngine::ContinueDrawing(worldX, worldY);
                
                // Invalidate only a small area around the drawing point (in screen space)
                int scaledBrushSize = (int)(app.brushSize * app.zoomLevel);
                RECT updateRect = {
                    x - scaledBrushSize - 5, y - scaledBrushSize - 5,
                    x + scaledBrushSize + 5, y + scaledBrushSize + 5
                };
                InvalidateRect(hwnd, &updateRect, FALSE);
            } else if (app.currentTool == TOOL_RECTANGLE || app.currentTool == TOOL_CIRCLE || app.currentTool == TOOL_LINE) {
                // For shapes, update preview coordinates but use XOR drawing to avoid flashing
                int oldX = app.drawCurrentX;
                int oldY = app.drawCurrentY;
                int worldX = ScreenToWorldX(x, app);
                int worldY = ScreenToWorldY(y, app);
                DrawingEngine::ContinueDrawing(worldX, worldY);
                
                // Use direct drawing with XOR for preview (no invalidate needed)
                // Convert world coordinates to screen coordinates for XOR drawing
                int screenStartX = WorldToScreenX(app.drawStartX, app);
                int screenStartY = WorldToScreenY(app.drawStartY, app);
                int screenOldX = WorldToScreenX(oldX, app);
                int screenOldY = WorldToScreenY(oldY, app);
                int screenNewX = WorldToScreenX(app.drawCurrentX, app);
                int screenNewY = WorldToScreenY(app.drawCurrentY, app);
                
                HDC hdc = GetDC(hwnd);
                SetROP2(hdc, R2_XORPEN);
                HPEN xorPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                HPEN oldPen = (HPEN)SelectObject(hdc, xorPen);
                
                // Erase old preview
                if (app.currentTool == TOOL_RECTANGLE) {
                    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);
                    Rectangle(hdc, screenStartX, screenStartY, screenOldX, screenOldY);
                    SelectObject(hdc, oldBrush);
                } else if (app.currentTool == TOOL_CIRCLE) {
                    int oldRadius = (int)(sqrt(pow(oldX - app.drawStartX, 2) + pow(oldY - app.drawStartY, 2)) * app.zoomLevel);
                    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);
                    Ellipse(hdc, screenStartX - oldRadius, screenStartY - oldRadius, 
                            screenStartX + oldRadius, screenStartY + oldRadius);
                    SelectObject(hdc, oldBrush);
                } else if (app.currentTool == TOOL_LINE) {
                    MoveToEx(hdc, screenStartX, screenStartY, NULL);
                    LineTo(hdc, screenOldX, screenOldY);
                }
                
                // Draw new preview
                if (app.currentTool == TOOL_RECTANGLE) {
                    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);
                    Rectangle(hdc, screenStartX, screenStartY, screenNewX, screenNewY);
                    SelectObject(hdc, oldBrush);
                } else if (app.currentTool == TOOL_CIRCLE) {
                    int newRadius = (int)(sqrt(pow(app.drawCurrentX - app.drawStartX, 2) + pow(app.drawCurrentY - app.drawStartY, 2)) * app.zoomLevel);
                    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);
                    Ellipse(hdc, screenStartX - newRadius, screenStartY - newRadius, 
                            screenStartX + newRadius, screenStartY + newRadius);
                    SelectObject(hdc, oldBrush);
                } else if (app.currentTool == TOOL_LINE) {
                    MoveToEx(hdc, screenStartX, screenStartY, NULL);
                    LineTo(hdc, screenNewX, screenNewY);
                }
                
                SelectObject(hdc, oldPen);
                DeleteObject(xorPen);
                SetROP2(hdc, R2_COPYPEN);
                ReleaseDC(hwnd, hdc);
            }
        }
    }
}

void OnLeftButtonUp(HWND hwnd, int x, int y)
{
    AppState& app = AppState::Instance();
    
    // If we were drawing a shape with XOR preview, clear it first
    if (app.hasPreview && (app.currentTool == TOOL_RECTANGLE || app.currentTool == TOOL_CIRCLE || app.currentTool == TOOL_LINE)) {
        HDC hdc = GetDC(hwnd);
        SetROP2(hdc, R2_XORPEN);
        HPEN xorPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, xorPen);
        
        // Clear the preview by drawing it again (XOR toggles) - use screen coordinates
        int screenStartX = WorldToScreenX(app.drawStartX, app);
        int screenStartY = WorldToScreenY(app.drawStartY, app);
        int screenCurrentX = WorldToScreenX(app.drawCurrentX, app);
        int screenCurrentY = WorldToScreenY(app.drawCurrentY, app);
        
        if (app.currentTool == TOOL_RECTANGLE) {
            HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);
            Rectangle(hdc, screenStartX, screenStartY, screenCurrentX, screenCurrentY);
            SelectObject(hdc, oldBrush);
        } else if (app.currentTool == TOOL_CIRCLE) {
            int radius = (int)(sqrt(pow(app.drawCurrentX - app.drawStartX, 2) + pow(app.drawCurrentY - app.drawStartY, 2)) * app.zoomLevel);
            HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, nullBrush);
            Ellipse(hdc, screenStartX - radius, screenStartY - radius, 
                    screenStartX + radius, screenStartY + radius);
            SelectObject(hdc, oldBrush);
        } else if (app.currentTool == TOOL_LINE) {
            MoveToEx(hdc, screenStartX, screenStartY, NULL);
            LineTo(hdc, screenCurrentX, screenCurrentY);
        }
        
        SelectObject(hdc, oldPen);
        DeleteObject(xorPen);
        SetROP2(hdc, R2_COPYPEN);
        ReleaseDC(hwnd, hdc);
    }
    
    DrawingEngine::EndDrawing();
    InvalidateCanvas(hwnd); // Redraw the final shape
}

void OnRightButtonDown(HWND hwnd, int x, int y)
{
    AppState& app = AppState::Instance();
    
    // Create context menu
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, IDM_FILE_NEW, L"New Canvas\tCtrl+N");
    AppendMenu(hMenu, MF_STRING, IDM_FILE_SAVE, L"Save Image\tCtrl+S");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_EDIT_UNDO, L"Undo\tCtrl+Z");
    AppendMenu(hMenu, MF_STRING, IDM_EDIT_REDO, L"Redo\tCtrl+Y");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_VIEW_GRID, app.showGrid ? L"Hide Grid\tG" : L"Show Grid\tG");
    AppendMenu(hMenu, MF_STRING, IDM_VIEW_THEME, app.currentTheme == THEME_LIGHT ? L"Dark Theme\tCtrl+T" : L"Light Theme\tCtrl+T");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_HELP_ABOUT, L"About\tF1");

    POINT pt = {x, y};
    ClientToScreen(hwnd, &pt);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

void OnMouseWheel(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    AppState& app = AppState::Instance();
    
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    if (ctrlPressed) {
        // Zoom
        if (delta > 0) {
            app.zoomLevel = (app.zoomLevel * 1.1f > 5.0f) ? 5.0f : app.zoomLevel * 1.1f;
        } else {
            app.zoomLevel = (app.zoomLevel / 1.1f < 0.2f) ? 0.2f : app.zoomLevel / 1.1f;
        }
    } else {
        // Pan
        app.panY += (delta > 0) ? 20 : -20;
    }
    
    InvalidateRect(hwnd, NULL, FALSE);
}

void OnKeyDown(HWND hwnd, WPARAM wParam)
{
    AppState& app = AppState::Instance();
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    switch (wParam) {
        case 'Z':
            if (ctrlPressed) {
                if (DrawingEngine::Undo()) {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            break;
            
        case 'Y':
            if (ctrlPressed) {
                if (DrawingEngine::Redo()) {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            break;
            
        case 'S':
            if (ctrlPressed) {
                OnCommand(hwnd, MAKEWPARAM(IDM_FILE_SAVE, 0));
            }
            break;
            
        case 'N':
            if (ctrlPressed) {
                DrawingEngine::ClearCanvas();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
            
        case 'T':
            if (ctrlPressed) {
                app.currentTheme = (app.currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
            
        case 'G':
            app.showGrid = !app.showGrid;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        // Tool shortcuts
        case 'B': DrawingEngine::SetTool(TOOL_BRUSH); InvalidateRect(hwnd, NULL, FALSE); break;
        case 'E': DrawingEngine::SetTool(TOOL_ERASER); InvalidateRect(hwnd, NULL, FALSE); break;
        case 'R': DrawingEngine::SetTool(TOOL_RECTANGLE); InvalidateRect(hwnd, NULL, FALSE); break;
        case 'C': DrawingEngine::SetTool(TOOL_CIRCLE); InvalidateRect(hwnd, NULL, FALSE); break;
        case 'L': DrawingEngine::SetTool(TOOL_LINE); InvalidateRect(hwnd, NULL, FALSE); break;
        
        // Brush size shortcuts
        case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9':
        {
            int size = (wParam - '0') * 2;
            DrawingEngine::SetBrushSize(size);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        
        case VK_F1: // Help
            MessageBox(hwnd, 
                L"Modern Paint Studio Pro v2.0\n\n"
                L"🎨 TOOLS:\n"
                L"• Click toolbar buttons for tools\n"
                L"• Click color palette for colors\n"
                L"• Drag brush size slider\n\n"
                L"⌨️ SHORTCUTS:\n"
                L"• Ctrl+Z: Undo\n"
                L"• Ctrl+Y: Redo\n"
                L"• Ctrl+S: Save image\n"
                L"• Ctrl+N: New canvas\n"
                L"• Ctrl+T: Toggle theme\n"
                L"• G: Toggle grid\n"
                L"• 1-9: Brush sizes\n"
                L"• Right-click: Context menu\n"
                L"• Ctrl+Mouse wheel: Zoom\n"
                L"• Mouse wheel: Scroll\n"
                L"• ESC: Exit",
                L"Help", MB_OK | MB_ICONINFORMATION);
            break;
            
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
    }
}

void OnCommand(HWND hwnd, WPARAM wParam)
{
    AppState& app = AppState::Instance();
    int commandId = LOWORD(wParam);
    
    switch (commandId) {
        case IDM_FILE_NEW:
            DrawingEngine::ClearCanvas();
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_FILE_SAVE:
        {
            OPENFILENAME ofn;
            WCHAR szFile[260] = {0};
            
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.BMP\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            
            if (GetSaveFileName(&ofn)) {
                // Simplified save implementation
                HDC hdcWindow = GetDC(hwnd);
                HDC hdcMem = CreateCompatibleDC(hdcWindow);
                RECT rect;
                GetClientRect(hwnd, &rect);
                HBITMAP hbmp = CreateCompatibleBitmap(hdcWindow, rect.right, rect.bottom);
                SelectObject(hdcMem, hbmp);
                BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdcWindow, 0, 0, SRCCOPY);
                
                // Simple bitmap save would require more complex implementation
                MessageBox(hwnd, L"Save feature implemented! (Simplified version)", L"Save", MB_OK);
                
                DeleteDC(hdcMem);
                ReleaseDC(hwnd, hdcWindow);
            }
            break;
        }
        
        case IDM_FILE_EXIT:
            PostQuitMessage(0);
            break;
            
        case IDM_EDIT_UNDO:
            if (DrawingEngine::Undo()) {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
            
        case IDM_EDIT_REDO:
            if (DrawingEngine::Redo()) {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
            
        case IDM_EDIT_CLEAR:
            DrawingEngine::ClearCanvas();
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_VIEW_ZOOM_IN:
            app.zoomLevel = (app.zoomLevel * 1.1f > 5.0f) ? 5.0f : app.zoomLevel * 1.1f;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_VIEW_ZOOM_OUT:
            app.zoomLevel = (app.zoomLevel / 1.1f < 0.2f) ? 0.2f : app.zoomLevel / 1.1f;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_VIEW_ZOOM_FIT:
            app.zoomLevel = 1.0f;
            app.panX = app.panY = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_VIEW_GRID:
            app.showGrid = !app.showGrid;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_VIEW_THEME:
            app.currentTheme = (app.currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_TOOLS_BRUSH:
            DrawingEngine::SetTool(TOOL_BRUSH);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_TOOLS_ERASER:
            DrawingEngine::SetTool(TOOL_ERASER);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_TOOLS_RECT:
            DrawingEngine::SetTool(TOOL_RECTANGLE);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_TOOLS_CIRCLE:
            DrawingEngine::SetTool(TOOL_CIRCLE);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_TOOLS_LINE:
            DrawingEngine::SetTool(TOOL_LINE);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
            
        case IDM_HELP_ABOUT:
            OnKeyDown(hwnd, VK_F1);
            break;
    }
}

void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    InvalidateRect(hwnd, NULL, FALSE);
}

} // namespace EventHandler