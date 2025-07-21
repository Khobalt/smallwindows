#include "../../include/event_handler.h"
#include "../../include/config.h"
#include "../../include/app_state.h"
#include "../../include/ui_renderer.h"
#include "../../include/drawing_engine.h"
#include "../../include/gpu_renderer.h"

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
            EventHandler::OnMouseHover(hwnd, LOWORD(lParam), HIWORD(lParam));
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
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    // Check if GPU rendering is available
    bool useGPU = GPURenderer::GPURenderingEngine::GetContext().initialized;
    
    if (useGPU) {
        OnPaintGPU(hwnd, clientRect);
    } else {
        OnPaintSoftware(hdc, clientRect);
    }
    
    EndPaint(hwnd, &ps);
}

void OnPaintGPU(HWND hwnd, RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    // Begin GPU rendering
    GPURenderer::GPURenderingEngine::BeginDraw();
    
    // Clear background with GPU
    COLORREF bgColor = (app.currentTheme == THEME_LIGHT) ? RGB(255, 255, 255) : RGB(30, 30, 30);
    GPURenderer::GPURenderingEngine::Clear(bgColor);
    
    // Set up zoom and pan transform
    D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Scale(app.zoomLevel, app.zoomLevel) *
                                   D2D1::Matrix3x2F::Translation((float)app.panX, (float)(app.panY + TOOLBAR_HEIGHT));
    GPURenderer::GPURenderingEngine::SetTransform(transform);
    
    // Draw grid if enabled - GPU accelerated!
    if (app.showGrid) {
        DrawGridGPU(clientRect);
    }
    
    // Draw all drawing points - GPU accelerated!
    DrawPointsGPU();
    
    // Reset transform for UI elements
    GPURenderer::GPURenderingEngine::ResetTransform();
    
    // Draw UI elements - GPU accelerated!
    UIRenderer::DrawToolbarGPU(clientRect);
    UIRenderer::DrawStatusBarGPU(clientRect);
    
    // Draw advanced color picker if visible - GPU accelerated!
    if (app.showAdvancedColorPicker) {
        UIRenderer::DrawAdvancedColorPickerGPU(clientRect);
    }
    
    // End GPU rendering
    GPURenderer::GPURenderingEngine::EndDraw();
}

void OnPaintSoftware(HDC hdc, RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    // Double buffering: Create memory DC and bitmap
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    // Clear background on memory DC
    COLORREF bgColor = (app.currentTheme == THEME_LIGHT) ? RGB(255, 255, 255) : RGB(30, 30, 30);
    HBRUSH bgBrush = CreateSolidBrush(bgColor);
    FillRect(memDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw grid if enabled
    if (app.showGrid) {
        HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        HPEN oldPen = (HPEN)SelectObject(memDC, gridPen);
        
        int gridSize = (int)(20 * app.zoomLevel);
        for (int x = app.panX % gridSize; x < clientRect.right; x += gridSize) {
            MoveToEx(memDC, x, TOOLBAR_HEIGHT, NULL);
            LineTo(memDC, x, clientRect.bottom - STATUSBAR_HEIGHT);
        }
        for (int y = TOOLBAR_HEIGHT + (app.panY % gridSize); y < clientRect.bottom - STATUSBAR_HEIGHT; y += gridSize) {
            MoveToEx(memDC, 0, y, NULL);
            LineTo(memDC, clientRect.right, y);
        }
        
        SelectObject(memDC, oldPen);
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
                HPEN oldPen = (HPEN)SelectObject(memDC, strokePen);
                
                // Draw thick line between points
                MoveToEx(memDC, prevX, prevY, NULL);
                LineTo(memDC, currX, currY);
                
                // Draw circular brush at current point for smooth appearance
                HBRUSH pointBrush = CreateSolidBrush(point.color);
                HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, pointBrush);
                SelectObject(memDC, GetStockObject(NULL_PEN)); // No outline
                
                Ellipse(memDC, 
                        currX - scaledBrushSize/2, 
                        currY - scaledBrushSize/2,
                        currX + scaledBrushSize/2, 
                        currY + scaledBrushSize/2);
                
                SelectObject(memDC, oldBrush);
                DeleteObject(pointBrush);
                SelectObject(memDC, oldPen);
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
                HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, pointBrush);
                SelectObject(memDC, GetStockObject(NULL_PEN)); // No outline
                
                Ellipse(memDC, 
                        x - scaledBrushSize/2, 
                        y - scaledBrushSize/2,
                        x + scaledBrushSize/2, 
                        y + scaledBrushSize/2);
                
                SelectObject(memDC, oldBrush);
                DeleteObject(pointBrush);
            }
        }
    }
    
    // Draw UI elements on memory DC
    UIRenderer::DrawToolbar(memDC, clientRect);
    UIRenderer::DrawStatusBar(memDC, clientRect);
    
    if (app.showAdvancedColorPicker) {
        UIRenderer::DrawAdvancedColorPicker(memDC);
    }
    
    // Blit the memory DC to the screen DC (eliminates flicker!)
    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
    
    // Clean up double buffering resources
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
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
            InvalidateToolbar(hwnd); // Only toolbar needs redraw for color picker toggle
        } else if (x >= 600 && x <= 700) { // Brush size slider
            int newSize = 1 + ((x - 600) * 19) / 100;
            DrawingEngine::SetBrushSize(newSize);
            InvalidateToolbar(hwnd);
            InvalidateStatusBar(hwnd);
        } else if (x >= 750 && x <= 820) { // Theme toggle
            app.currentTheme = (app.currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
            InvalidateRect(hwnd, NULL, FALSE); // Theme affects entire screen - keep full redraw
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
    AppendMenu(hMenu, MF_STRING, IDM_FILE_OPEN, L"Open File\tCtrl+O");
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
            
        case 'O':
            if (ctrlPressed) {
                OnCommand(hwnd, MAKEWPARAM(IDM_FILE_OPEN, 0));
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
            InvalidateCanvas(hwnd);  // Only canvas needs redraw for grid
            InvalidateStatusBar(hwnd);  // Status bar shows grid state
            break;
            
        // Tool shortcuts
        case 'B': DrawingEngine::SetTool(TOOL_BRUSH); InvalidateToolbar(hwnd); InvalidateStatusBar(hwnd); break;
        case 'E': DrawingEngine::SetTool(TOOL_ERASER); InvalidateToolbar(hwnd); InvalidateStatusBar(hwnd); break;
        case 'R': DrawingEngine::SetTool(TOOL_RECTANGLE); InvalidateToolbar(hwnd); InvalidateStatusBar(hwnd); break;
        case 'C': DrawingEngine::SetTool(TOOL_CIRCLE); InvalidateToolbar(hwnd); InvalidateStatusBar(hwnd); break;
        case 'L': DrawingEngine::SetTool(TOOL_LINE); InvalidateToolbar(hwnd); InvalidateStatusBar(hwnd); break;
        
        // Brush size shortcuts
        case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9':
        {
            int size = (wParam - '0') * 2;
            DrawingEngine::SetBrushSize(size);
            InvalidateToolbar(hwnd);
            InvalidateStatusBar(hwnd);
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
            ofn.lpstrFilter = L"Paint Studio Files (*.mpsp)\0*.MPSP\0PNG Files (*.png)\0*.PNG\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            
            if (GetSaveFileName(&ofn)) {
                std::string filename;
                filename.resize(WideCharToMultiByte(CP_UTF8, 0, szFile, -1, NULL, 0, NULL, NULL));
                WideCharToMultiByte(CP_UTF8, 0, szFile, -1, &filename[0], filename.size(), NULL, NULL);
                filename.resize(strlen(filename.c_str())); // Remove null terminator
                
                // Determine file type from filter index and ensure proper extension
                bool success = false;
                if (ofn.nFilterIndex == 1) {
                    // Save as native format - ensure .mpsp extension
                    filename = DrawingEngine::EnsureFileExtension(filename, ".mpsp");
                    success = DrawingEngine::SaveDrawing(filename);
                } else if (ofn.nFilterIndex == 2) {
                    // Export as PNG - ensure .png extension
                    filename = DrawingEngine::EnsureFileExtension(filename, ".png");
                    RECT rect;
                    GetClientRect(hwnd, &rect);
                    success = DrawingEngine::ExportAsBitmap(filename, rect.right, rect.bottom - TOOLBAR_HEIGHT - STATUSBAR_HEIGHT);
                } else {
                    // All files - determine by existing extension or default to native format
                    if (filename.find(".png") != std::string::npos) {
                        filename = DrawingEngine::EnsureFileExtension(filename, ".png");
                        RECT rect;
                        GetClientRect(hwnd, &rect);
                        success = DrawingEngine::ExportAsBitmap(filename, rect.right, rect.bottom - TOOLBAR_HEIGHT - STATUSBAR_HEIGHT);
                    } else {
                        filename = DrawingEngine::EnsureFileExtension(filename, ".mpsp");
                        success = DrawingEngine::SaveDrawing(filename);
                    }
                }
                
                if (success) {
                    MessageBox(hwnd, L"File saved successfully!", L"Save", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBox(hwnd, L"Failed to save file!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }
        
        case IDM_FILE_OPEN:
        {
            OPENFILENAME ofn;
            WCHAR szFile[260] = {0};
            
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Paint Studio Files (*.mpsp)\0*.MPSP\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            
            if (GetOpenFileName(&ofn)) {
                std::string filename;
                filename.resize(WideCharToMultiByte(CP_UTF8, 0, szFile, -1, NULL, 0, NULL, NULL));
                WideCharToMultiByte(CP_UTF8, 0, szFile, -1, &filename[0], filename.size(), NULL, NULL);
                filename.resize(strlen(filename.c_str())); // Remove null terminator
                
                if (DrawingEngine::LoadDrawing(filename)) {
                    InvalidateRect(hwnd, NULL, FALSE);
                    MessageBox(hwnd, L"File loaded successfully!", L"Open", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBox(hwnd, L"Failed to load file!", L"Error", MB_OK | MB_ICONERROR);
                }
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

void OnMouseHover(HWND hwnd, int x, int y)
{
    AppState& app = AppState::Instance();
    bool needsRedraw = false;
    
    // Reset hover states
    int oldHoveredTool = app.hoveredTool;
    bool oldHoveredAdvancedPicker = app.hoveredAdvancedPicker;
    bool oldHoveredThemeButton = app.hoveredThemeButton;
    
    app.hoveredTool = -1;
    app.hoveredAdvancedPicker = false;
    app.hoveredThemeButton = false;
    
    // Check if mouse is in toolbar area
    if (y >= 5 && y <= 35) {
        // Check tool buttons (6 tools, 50px wide each, starting at x=5)
        for (int i = 0; i < 6; i++) {
            RECT buttonRect = {i * 50 + 5, 5, i * 50 + 45, 35};
            if (x >= buttonRect.left && x <= buttonRect.right) {
                app.hoveredTool = i;
                break;
            }
        }
        
        // Check advanced color picker button (x=545-590)
        if (x >= 545 && x <= 590) {
            app.hoveredAdvancedPicker = true;
        }
        
        // Check theme button (x=750-820)
        if (x >= 750 && x <= 820) {
            app.hoveredThemeButton = true;
        }
    }
    
    // Only redraw toolbar if hover states changed
    if (oldHoveredTool != app.hoveredTool || 
        oldHoveredAdvancedPicker != app.hoveredAdvancedPicker ||
        oldHoveredThemeButton != app.hoveredThemeButton) {
        
        // Invalidate just the toolbar area for efficiency
        RECT toolbarRect = {0, 0, 1000, TOOLBAR_HEIGHT};
        InvalidateRect(hwnd, &toolbarRect, FALSE);
    }
}

void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    InvalidateRect(hwnd, NULL, FALSE);
}

void DrawGridGPU(RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    COLORREF gridColor = (app.currentTheme == THEME_LIGHT) ? RGB(200, 200, 200) : RGB(100, 100, 100);
    float gridSize = 20.0f * app.zoomLevel;
    
    // Draw vertical grid lines
    for (float x = (float)(app.panX % (int)gridSize); x < clientRect.right; x += gridSize) {
        GPURenderer::GPURenderingEngine::DrawLine(
            x, (float)TOOLBAR_HEIGHT, 
            x, (float)(clientRect.bottom - STATUSBAR_HEIGHT), 
            gridColor, 1.0f
        );
    }
    
    // Draw horizontal grid lines  
    for (float y = (float)(TOOLBAR_HEIGHT + (app.panY % (int)gridSize)); y < clientRect.bottom - STATUSBAR_HEIGHT; y += gridSize) {
        GPURenderer::GPURenderingEngine::DrawLine(
            0, y, 
            (float)clientRect.right, y, 
            gridColor, 1.0f
        );
    }
}

void DrawPointsGPU()
{
    AppState& app = AppState::Instance();
    
    if (app.drawingPoints.empty()) return;
    
    // Convert drawing points to GPU format and render them
    std::vector<D2D1_POINT_2F> currentStroke;
    DrawPoint* prevPoint = nullptr;
    
    for (size_t i = 0; i < app.drawingPoints.size(); i++) {
        const DrawPoint& point = app.drawingPoints[i];
        
        if (point.isStart) {
            // Finish previous stroke if any
            if (!currentStroke.empty()) {
                GPURenderer::GPURenderingEngine::DrawBrushStroke(
                    currentStroke, prevPoint->color, (float)prevPoint->brushSize
                );
                currentStroke.clear();
            }
            
            // Start new stroke
            currentStroke.push_back(D2D1::Point2F((float)point.x, (float)point.y));
            prevPoint = const_cast<DrawPoint*>(&point);
        } else if (prevPoint != nullptr) {
            // Continue stroke
            currentStroke.push_back(D2D1::Point2F((float)point.x, (float)point.y));
            prevPoint = const_cast<DrawPoint*>(&point);
        }
    }
    
    // Finish last stroke
    if (!currentStroke.empty() && prevPoint) {
        GPURenderer::GPURenderingEngine::DrawBrushStroke(
            currentStroke, prevPoint->color, (float)prevPoint->brushSize
        );
    }
}

} // namespace EventHandler