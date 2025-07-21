#include "../../include/ui_renderer.h"
#include "../../include/config.h"
#include "../../include/app_state.h"
#include "../../include/drawing_engine.h"
#include "../../include/icon_resources.h"

namespace UIRenderer {

void DrawToolbar(HDC hdc, RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    COLORREF toolbarBg = (app.currentTheme == THEME_LIGHT) ? RGB(240, 240, 240) : RGB(37, 37, 38);
    COLORREF toolbarText = (app.currentTheme == THEME_LIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255);
    COLORREF buttonBg = (app.currentTheme == THEME_LIGHT) ? RGB(225, 225, 225) : RGB(60, 60, 60);
    COLORREF activeBg = RGB(0, 120, 215);
    
    // Toolbar background
    RECT toolbarRect = {0, 0, clientRect.right, TOOLBAR_HEIGHT};
    HBRUSH toolbarBrush = CreateSolidBrush(toolbarBg);
    FillRect(hdc, &toolbarRect, toolbarBrush);
    DeleteObject(toolbarBrush);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, toolbarText);
    
    // Tool buttons with icons and hover effects
    for (int i = 0; i < 6; i++) {
        RECT buttonRect = {i * 50 + 5, 5, i * 50 + 45, 35};
        
        // Determine button colors based on state
        COLORREF btnColor;
        if (app.currentTool == i) {
            btnColor = activeBg;  // Active tool - blue
        } else if (app.hoveredTool == i) {
            btnColor = (app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 255) : RGB(80, 80, 100);  // Hover - light blue tint
        } else {
            btnColor = buttonBg;  // Normal state
        }
        
        HBRUSH btnBrush = CreateSolidBrush(btnColor);
        FillRect(hdc, &buttonRect, btnBrush);
        DeleteObject(btnBrush);
        
        // Button border with enhanced style
        int borderWidth = (app.currentTool == i) ? 2 : 1;
        COLORREF borderColor;
        if (app.currentTool == i) {
            borderColor = RGB(255, 255, 255);  // Active - white border
        } else if (app.hoveredTool == i) {
            borderColor = RGB(180, 180, 180);  // Hover - lighter border
        } else {
            borderColor = RGB(128, 128, 128);  // Normal - gray border
        }
        
        HPEN borderPen = CreatePen(PS_SOLID, borderWidth, borderColor);
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        Rectangle(hdc, buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.bottom);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);
        
        // Draw tool icon with appropriate color
        const IconResources::Icon* icon = IconResources::GetToolIcon((ToolType)i);
        if (icon) {
            // Center the 16x16 icon in the 40x30 button
            int iconX = buttonRect.left + (40 - 16) / 2;
            int iconY = buttonRect.top + (30 - 16) / 2;
            
            COLORREF iconColor;
            if (app.currentTool == i) {
                iconColor = RGB(255, 255, 255);  // Active - white icon
            } else if (app.hoveredTool == i) {
                iconColor = (app.currentTheme == THEME_LIGHT) ? RGB(50, 50, 150) : RGB(200, 200, 255);  // Hover - darker/lighter
            } else {
                iconColor = toolbarText;  // Normal - theme text color
            }
            
            IconResources::DrawIcon(hdc, iconX, iconY, icon, iconColor, 1);
        }
    }
    
    // Color palette
    TextOut(hdc, 310, 10, L"Colors:", 7);
    for (int i = 0; i < 16; i++) {
        RECT colorRect = {350 + i * 12, 10, 350 + i * 12 + 10, 20};
        HBRUSH colorBrush = CreateSolidBrush(colorPalette[i]);
        FillRect(hdc, &colorRect, colorBrush);
        DeleteObject(colorBrush);
        
        // Highlight current color
        if (colorPalette[i] == app.currentColor) {
            HPEN highlightPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
            HPEN oldPen = (HPEN)SelectObject(hdc, highlightPen);
            Rectangle(hdc, colorRect.left-1, colorRect.top-1, colorRect.right+1, colorRect.bottom+1);
            SelectObject(hdc, oldPen);
            DeleteObject(highlightPen);
        }
    }
    
    // Advanced color picker button with hover effect
    RECT pickerButtonRect = {545, 5, 590, 35};
    COLORREF pickerBtnColor;
    if (app.showAdvancedColorPicker) {
        pickerBtnColor = activeBg;  // Active - blue
    } else if (app.hoveredAdvancedPicker) {
        pickerBtnColor = (app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 235) : RGB(70, 70, 70);  // Hover - lighter
    } else {
        pickerBtnColor = buttonBg;  // Normal
    }
    
    HBRUSH pickerBtnBrush = CreateSolidBrush(pickerBtnColor);
    FillRect(hdc, &pickerButtonRect, pickerBtnBrush);
    DeleteObject(pickerBtnBrush);
    
    COLORREF pickerBorderColor = app.hoveredAdvancedPicker ? RGB(180, 180, 180) : RGB(128, 128, 128);
    HPEN borderPen2 = CreatePen(PS_SOLID, 1, pickerBorderColor);
    HPEN oldPen2 = (HPEN)SelectObject(hdc, borderPen2);
    Rectangle(hdc, pickerButtonRect.left, pickerButtonRect.top, pickerButtonRect.right, pickerButtonRect.bottom);
    SelectObject(hdc, oldPen2);
    DeleteObject(borderPen2);
    
    RECT pickerTextRect = pickerButtonRect;
    pickerTextRect.top += 8;
    COLORREF pickerTextColor = app.showAdvancedColorPicker ? RGB(255, 255, 255) : toolbarText;
    SetTextColor(hdc, pickerTextColor);
    DrawText(hdc, L"More", -1, &pickerTextRect, DT_CENTER);
    SetTextColor(hdc, toolbarText);  // Reset text color
    
    // Brush size slider
    TextOut(hdc, 560, 10, L"Size:", 5);
    RECT sliderRect = {600, 15, 700, 25};
    HBRUSH sliderBrush = CreateSolidBrush(buttonBg);
    FillRect(hdc, &sliderRect, sliderBrush);
    DeleteObject(sliderBrush);
    
    // Size indicator
    int sliderPos = 600 + ((app.brushSize - 1) * 100) / 19;
    RECT sizeIndicator = {sliderPos - 2, 12, sliderPos + 2, 28};
    HBRUSH sizeBrush = CreateSolidBrush(activeBg);
    FillRect(hdc, &sizeIndicator, sizeBrush);
    DeleteObject(sizeBrush);
    
    // Theme toggle button with hover effect
    RECT themeRect = {750, 5, 820, 35};
    COLORREF themeBtnColor = app.hoveredThemeButton ? 
        ((app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 235) : RGB(70, 70, 70)) : buttonBg;
    HBRUSH themeBrush = CreateSolidBrush(themeBtnColor);
    FillRect(hdc, &themeRect, themeBrush);
    DeleteObject(themeBrush);
    
    COLORREF themeBorderColor = app.hoveredThemeButton ? RGB(180, 180, 180) : RGB(128, 128, 128);
    HPEN borderPen = CreatePen(PS_SOLID, 1, themeBorderColor);
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    Rectangle(hdc, themeRect.left, themeRect.top, themeRect.right, themeRect.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
    
    const WCHAR* themeText = (app.currentTheme == THEME_LIGHT) ? L"Dark" : L"Light";
    RECT themeTextRect = themeRect;
    themeTextRect.top += 8;
    DrawText(hdc, themeText, -1, &themeTextRect, DT_CENTER);
}

void DrawStatusBar(HDC hdc, RECT clientRect)
{
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

void DrawAdvancedColorPicker(HDC hdc)
{
    AppState& app = AppState::Instance();
    
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
    
    // Draw color wheel pixels
    for (int y = -radius; y <= radius; y += 2) {
        for (int x = -radius; x <= radius; x += 2) {
            float distance = sqrt(x * x + y * y);
            if (distance <= radius) {
                float angle = atan2(y, x) * 180.0f / 3.14159f;
                if (angle < 0) angle += 360;
                float saturation = distance / radius;
                
                COLORREF color = DrawingEngine::HSVtoRGB(angle, saturation, 0.9f);
                SetPixel(hdc, centerX + x, centerY + y, color);
            }
        }
    }
    
    // Current color display
    RECT currentColorRect = {app.pickerX + 20, app.pickerY + 160, app.pickerX + 180, app.pickerY + 180};
    HBRUSH currentBrush = CreateSolidBrush(app.currentColor);
    FillRect(hdc, &currentColorRect, currentBrush);
    DeleteObject(currentBrush);
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