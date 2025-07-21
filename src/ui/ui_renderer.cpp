#include "../../include/ui_renderer.h"
#include "../../include/config.h"
#include "../../include/app_state.h"
#include "../../include/drawing_engine.h"
#include "../../include/icon_resources.h"
#include "../../include/gpu_renderer.h"

namespace UIRenderer {

void DrawToolbar(HDC hdc, RECT clientRect)
{
    // Check if GPU rendering is available and delegate accordingly
    if (GPURenderer::GPURenderingEngine::GetContext().initialized) {
        DrawToolbarGPU(clientRect);
    } else {
        DrawToolbarSoftware(hdc, clientRect);
    }
}

void DrawStatusBar(HDC hdc, RECT clientRect)
{
    // Check if GPU rendering is available and delegate accordingly
    if (GPURenderer::GPURenderingEngine::GetContext().initialized) {
        DrawStatusBarGPU(clientRect);
    } else {
        // Original software-based implementation (fallback)
        AppState& app = AppState::Instance();
        
        COLORREF statusBg = (app.currentTheme == THEME_LIGHT) ? RGB(240, 240, 240) : RGB(37, 37, 38);
        RECT statusRect = {0, clientRect.bottom - STATUSBAR_HEIGHT, clientRect.right, clientRect.bottom};
        HBRUSH statusBrush = CreateSolidBrush(statusBg);
        FillRect(hdc, &statusRect, statusBrush);
        DeleteObject(statusBrush);
        
        SetBkMode(hdc, TRANSPARENT);
        COLORREF statusText = (app.currentTheme == THEME_LIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255);
        SetTextColor(hdc, statusText);
        
        WCHAR statusText1[200];
        const WCHAR* toolName = (app.currentTool == TOOL_BRUSH) ? L"Brush" :
                              (app.currentTool == TOOL_ERASER) ? L"Eraser" :
                              (app.currentTool == TOOL_RECTANGLE) ? L"Rectangle" :
                              (app.currentTool == TOOL_CIRCLE) ? L"Circle" :
                              (app.currentTool == TOOL_LINE) ? L"Line" : L"Color Picker";
        
        swprintf(statusText1, 200, L"Tool: %s | Size: %d | Zoom: %.0f%% | Grid: %s | Theme: %s | Points: %zu | F1: Help", 
                toolName, app.brushSize, app.zoomLevel * 100,
                app.showGrid ? L"On" : L"Off",
                (app.currentTheme == THEME_LIGHT) ? L"Light" : L"Dark", 
                app.drawingPoints.size());
        
        TextOut(hdc, 10, clientRect.bottom - STATUSBAR_HEIGHT + 5, statusText1, wcslen(statusText1));
    }
}

void DrawAdvancedColorPicker(HDC hdc)
{
    AppState& app = AppState::Instance();
    
    // Check if GPU rendering is available and delegate accordingly
    if (GPURenderer::GPURenderingEngine::GetContext().initialized) {
        // Use client rect approximation for GPU version
        RECT clientRect = {0, 0, 800, 600}; // Default size, GPU version uses app state positions
        DrawAdvancedColorPickerGPU(clientRect);
    } else {
        // Original software-based implementation (fallback)
        // Background
        RECT pickerBg = {app.pickerX, app.pickerY, app.pickerX + 200, app.pickerY + 200};
        HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(hdc, &pickerBg, bgBrush);
        DeleteObject(bgBrush);
        
        // Border
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        Rectangle(hdc, pickerBg.left, pickerBg.top, pickerBg.right, pickerBg.bottom);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);
        
        // Title
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        RECT titleRect = {app.pickerX + 10, app.pickerY + 10, app.pickerX + 190, app.pickerY + 30};
        DrawText(hdc, L"Advanced Color Picker", -1, &titleRect, DT_CENTER);
        
        // HSV Color wheel (simplified)
        int centerX = app.pickerX + 100;
        int centerY = app.pickerY + 100;
        int radius = 80;
        
        // Draw color wheel with small rectangles instead of pixels
        for (int y = -radius; y <= radius; y += 2) {
            for (int x = -radius; x <= radius; x += 2) {
                float distance = sqrt(x * x + y * y);
                if (distance <= radius) {
                    float angle = atan2(y, x) * 180.0f / 3.14159f;
                    if (angle < 0) angle += 360;
                    float saturation = distance / radius;
                    
                    COLORREF color = DrawingEngine::HSVtoRGB(angle, saturation, 0.9f);
                    
                    // Draw small rectangle instead of pixel
                    HBRUSH colorBrush = CreateSolidBrush(color);
                    RECT pixelRect = {centerX + x, centerY + y, centerX + x + 2, centerY + y + 2};
                    FillRect(hdc, &pixelRect, colorBrush);
                    DeleteObject(colorBrush);
                }
            }
        }
        
        // Current color display
        RECT currentColorRect = {app.pickerX + 20, app.pickerY + 160, app.pickerX + 180, app.pickerY + 180};
        HBRUSH currentBrush = CreateSolidBrush(app.currentColor);
        FillRect(hdc, &currentColorRect, currentBrush);
        DeleteObject(currentBrush);
    }
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
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, L"&New\tCtrl+N");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, L"&Open\tCtrl+O");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, L"&Save\tCtrl+S");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVEAS, L"Save &As...");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"E&xit\tAlt+F4");

    // Edit Menu
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_UNDO, L"&Undo\tCtrl+Z");
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REDO, L"&Redo\tCtrl+Y");
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_CLEAR, L"&Clear Canvas\tCtrl+N");

    // View Menu
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_ZOOM_IN, L"Zoom &In\tCtrl++");
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_ZOOM_OUT, L"Zoom &Out\tCtrl+-");
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_ZOOM_FIT, L"&Fit to Window\tCtrl+0");
    AppendMenu(hViewMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_GRID, L"Show &Grid\tG");
    AppendMenu(hViewMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_THEME, L"Toggle &Theme\tCtrl+T");

    // Tools Menu
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_BRUSH, L"&Brush Tool\tB");
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_ERASER, L"&Eraser Tool\tE");
    AppendMenu(hToolsMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_RECT, L"&Rectangle Tool\tR");
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_CIRCLE, L"&Circle Tool\tC");
    AppendMenu(hToolsMenu, MF_STRING, IDM_TOOLS_LINE, L"&Line Tool\tL");

    // Help Menu
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, L"&About\tF1");

    // Append submenus to main menu
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, L"&Edit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hViewMenu, L"&View");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hToolsMenu, L"&Tools");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, L"&Help");

    return hMenuBar;
}


} // namespace UIRenderer