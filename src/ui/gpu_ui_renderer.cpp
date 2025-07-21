#include "../../include/ui_renderer.h"
#include "../../include/config.h"
#include "../../include/app_state.h"
#include "../../include/drawing_engine.h"
#include "../../include/icon_resources.h"
#include "../../include/gpu_renderer.h"

namespace UIRenderer {

void DrawToolbarGPU(RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    // Define colors
    COLORREF toolbarBg = (app.currentTheme == THEME_LIGHT) ? RGB(240, 240, 240) : RGB(37, 37, 38);
    COLORREF toolbarText = (app.currentTheme == THEME_LIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255);
    COLORREF buttonBg = (app.currentTheme == THEME_LIGHT) ? RGB(225, 225, 225) : RGB(60, 60, 60);
    COLORREF activeBg = RGB(0, 120, 215);
    
    // Draw toolbar background - GPU accelerated!
    GPURenderer::GPURenderingEngine::FillRectangle(
        0, 0, (float)clientRect.right, (float)TOOLBAR_HEIGHT, toolbarBg
    );
    
    // Tool buttons with icons and hover effects - GPU accelerated!
    for (int i = 0; i < 6; i++) {
        float buttonLeft = (float)(i * 50 + 5);
        float buttonTop = 5.0f;
        float buttonWidth = 40.0f;
        float buttonHeight = 30.0f;
        
        // Determine button colors based on state
        COLORREF btnColor;
        if (app.currentTool == i) {
            btnColor = activeBg;  // Active tool - blue
        } else if (app.hoveredTool == i) {
            btnColor = (app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 255) : RGB(80, 80, 100);  // Hover
        } else {
            btnColor = buttonBg;  // Normal state
        }
        
        // Fill button background - GPU accelerated!
        GPURenderer::GPURenderingEngine::FillRectangle(
            buttonLeft, buttonTop, buttonWidth, buttonHeight, btnColor
        );
        
        // Button border - GPU accelerated!
        float borderWidth = (app.currentTool == i) ? 2.0f : 1.0f;
        COLORREF borderColor;
        if (app.currentTool == i) {
            borderColor = RGB(255, 255, 255);  // Active - white border
        } else if (app.hoveredTool == i) {
            borderColor = RGB(180, 180, 180);  // Hover - lighter border
        } else {
            borderColor = RGB(128, 128, 128);  // Normal - gray border
        }
        
        GPURenderer::GPURenderingEngine::DrawRectangle(
            buttonLeft, buttonTop, buttonWidth, buttonHeight, borderColor, borderWidth
        );
        
        // Draw tool icon - GPU accelerated!
        const IconResources::Icon* icon = IconResources::GetToolIcon((ToolType)i);
        if (icon) {
            float iconX = buttonLeft + (buttonWidth - 16) / 2;
            float iconY = buttonTop + (buttonHeight - 16) / 2;
            
            COLORREF iconColor;
            if (app.currentTool == i) {
                iconColor = RGB(255, 255, 255);  // Active - white icon
            } else if (app.hoveredTool == i) {
                iconColor = (app.currentTheme == THEME_LIGHT) ? RGB(50, 50, 150) : RGB(200, 200, 255);
            } else {
                iconColor = toolbarText;  // Normal - theme text color
            }
            
            GPURenderer::GPURenderingEngine::DrawIcon(
                iconX, iconY, icon->data, icon->width, icon->height, iconColor, 1.0f
            );
        }
    }
    
    // Color palette - GPU accelerated!
    GPURenderer::GPURenderingEngine::DrawText(L"Colors:", 310, 10, 50, 20, toolbarText);
    
    for (int i = 0; i < 16; i++) {
        float colorX = (float)(350 + i * 12);
        float colorY = 10.0f;
        float colorSize = 10.0f;
        
        GPURenderer::GPURenderingEngine::FillRectangle(
            colorX, colorY, colorSize, colorSize, colorPalette[i]
        );
        
        // Highlight current color - GPU accelerated!
        if (colorPalette[i] == app.currentColor) {
            GPURenderer::GPURenderingEngine::DrawRectangle(
                colorX - 1, colorY - 1, colorSize + 2, colorSize + 2, RGB(255, 255, 0), 2.0f
            );
        }
    }
    
    // Advanced color picker button - GPU accelerated!
    float pickerLeft = 545.0f;
    float pickerTop = 5.0f;
    float pickerWidth = 45.0f;
    float pickerHeight = 30.0f;
    
    COLORREF pickerBtnColor;
    if (app.showAdvancedColorPicker) {
        pickerBtnColor = activeBg;
    } else if (app.hoveredAdvancedPicker) {
        pickerBtnColor = (app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 235) : RGB(70, 70, 70);
    } else {
        pickerBtnColor = buttonBg;
    }
    
    GPURenderer::GPURenderingEngine::FillRectangle(
        pickerLeft, pickerTop, pickerWidth, pickerHeight, pickerBtnColor
    );
    
    COLORREF pickerBorderColor = app.hoveredAdvancedPicker ? RGB(180, 180, 180) : RGB(128, 128, 128);
    GPURenderer::GPURenderingEngine::DrawRectangle(
        pickerLeft, pickerTop, pickerWidth, pickerHeight, pickerBorderColor, 1.0f
    );
    
    COLORREF pickerTextColor = app.showAdvancedColorPicker ? RGB(255, 255, 255) : toolbarText;
    GPURenderer::GPURenderingEngine::DrawText(L"More", pickerLeft, pickerTop + 8, pickerWidth, 15, pickerTextColor);
    
    // Brush size controls - GPU accelerated!
    GPURenderer::GPURenderingEngine::DrawText(L"Size:", 560, 10, 40, 20, toolbarText);
    
    // Size slider background
    GPURenderer::GPURenderingEngine::FillRectangle(600, 15, 100, 10, buttonBg);
    
    // Size indicator
    float sliderPos = 600 + ((app.brushSize - 1) * 100) / 19;
    GPURenderer::GPURenderingEngine::FillRectangle(sliderPos - 2, 12, 4, 16, activeBg);
    
    // Theme toggle button - GPU accelerated!
    float themeLeft = 750.0f;
    float themeTop = 5.0f;
    float themeWidth = 70.0f;
    float themeHeight = 30.0f;
    
    COLORREF themeBtnColor = app.hoveredThemeButton ? 
        ((app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 235) : RGB(70, 70, 70)) : buttonBg;
    
    GPURenderer::GPURenderingEngine::FillRectangle(
        themeLeft, themeTop, themeWidth, themeHeight, themeBtnColor
    );
    
    COLORREF themeBorderColor = app.hoveredThemeButton ? RGB(180, 180, 180) : RGB(128, 128, 128);
    GPURenderer::GPURenderingEngine::DrawRectangle(
        themeLeft, themeTop, themeWidth, themeHeight, themeBorderColor, 1.0f
    );
    
    const WCHAR* themeText = (app.currentTheme == THEME_LIGHT) ? L"Dark" : L"Light";
    GPURenderer::GPURenderingEngine::DrawText(themeText, themeLeft, themeTop + 8, themeWidth, 15, toolbarText);
}

void DrawToolbarSoftware(HDC hdc, RECT clientRect)
{
    // Original software-based implementation (fallback)
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
    
    // Rest of the original toolbar implementation...
    // (keeping this as fallback for compatibility)
    
    // Tool buttons with icons and hover effects
    for (int i = 0; i < 6; i++) {
        RECT buttonRect = {i * 50 + 5, 5, i * 50 + 45, 35};
        
        // Determine button colors based on state
        COLORREF btnColor;
        if (app.currentTool == i) {
            btnColor = activeBg;  // Active tool - blue
        } else if (app.hoveredTool == i) {
            btnColor = (app.currentTheme == THEME_LIGHT) ? RGB(235, 235, 255) : RGB(80, 80, 100);
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
                iconColor = (app.currentTheme == THEME_LIGHT) ? RGB(50, 50, 150) : RGB(200, 200, 255);
            } else {
                iconColor = toolbarText;  // Normal - theme text color
            }
            
            IconResources::DrawIcon(hdc, iconX, iconY, icon, iconColor, 1);
        }
    }
    
    // Color palette and other elements...
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
}

void DrawStatusBarGPU(RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    // Status bar colors
    COLORREF statusBg = (app.currentTheme == THEME_LIGHT) ? RGB(240, 240, 240) : RGB(37, 37, 38);
    COLORREF statusText = (app.currentTheme == THEME_LIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255);
    
    float statusTop = (float)(clientRect.bottom - STATUSBAR_HEIGHT);
    float statusHeight = (float)STATUSBAR_HEIGHT;
    
    // Status bar background - GPU accelerated!
    GPURenderer::GPURenderingEngine::FillRectangle(
        0, statusTop, (float)clientRect.right, statusHeight, statusBg
    );
    
    // Status text - GPU accelerated!
    const WCHAR* toolName = (app.currentTool == TOOL_BRUSH) ? L"Brush" :
                          (app.currentTool == TOOL_ERASER) ? L"Eraser" :
                          (app.currentTool == TOOL_RECTANGLE) ? L"Rectangle" :
                          (app.currentTool == TOOL_CIRCLE) ? L"Circle" :
                          (app.currentTool == TOOL_LINE) ? L"Line" : L"Color Picker";
    
    // Format status text
    WCHAR statusText1[200];
    swprintf(statusText1, 200, L"Tool: %s | Size: %d | Zoom: %.0f%% | Grid: %s | Theme: %s | Points: %zu | F1: Help", 
            toolName, app.brushSize, app.zoomLevel * 100,
            app.showGrid ? L"On" : L"Off",
            (app.currentTheme == THEME_LIGHT) ? L"Light" : L"Dark", 
            app.drawingPoints.size());
    
    // Draw status text with GPU acceleration
    GPURenderer::GPURenderingEngine::DrawText(
        statusText1, 
        10.0f, 
        statusTop + 5.0f, 
        (float)(clientRect.right - 20), 
        statusHeight - 10.0f, 
        statusText
    );
}

void DrawAdvancedColorPickerGPU(RECT clientRect)
{
    AppState& app = AppState::Instance();
    
    if (!app.showAdvancedColorPicker) return;
    
    float pickerX = (float)app.pickerX;
    float pickerY = (float)app.pickerY;
    float pickerWidth = 200.0f;
    float pickerHeight = 200.0f;
    
    // Background - GPU accelerated!
    GPURenderer::GPURenderingEngine::FillRectangle(
        pickerX, pickerY, pickerWidth, pickerHeight, RGB(240, 240, 240)
    );
    
    // Border - GPU accelerated!
    GPURenderer::GPURenderingEngine::DrawRectangle(
        pickerX, pickerY, pickerWidth, pickerHeight, RGB(100, 100, 100), 2.0f
    );
    
    // Title - GPU accelerated!
    GPURenderer::GPURenderingEngine::DrawText(
        L"Advanced Color Picker",
        pickerX + 10.0f,
        pickerY + 10.0f,
        pickerWidth - 20.0f,
        20.0f,
        RGB(0, 0, 0)
    );
    
    // Color wheel center
    float centerX = pickerX + 100.0f;
    float centerY = pickerY + 100.0f;
    float radius = 80.0f;
    
    // Draw color wheel with GPU rectangles (much faster than SetPixel!)
    for (int y = -80; y <= 80; y += 2) {
        for (int x = -80; x <= 80; x += 2) {
            float distance = sqrtf((float)(x * x + y * y));
            if (distance <= radius) {
                float angle = atan2f((float)y, (float)x) * 180.0f / 3.14159f;
                if (angle < 0) angle += 360.0f;
                float saturation = distance / radius;
                
                // Convert HSV to RGB (using our existing function)
                COLORREF color = DrawingEngine::HSVtoRGB(angle, saturation, 0.9f);
                
                // Draw 2x2 pixel rectangle - GPU accelerated!
                GPURenderer::GPURenderingEngine::FillRectangle(
                    centerX + (float)x, centerY + (float)y, 2.0f, 2.0f, color
                );
            }
        }
    }
    
    // Current color display - GPU accelerated!
    GPURenderer::GPURenderingEngine::FillRectangle(
        pickerX + 20.0f, pickerY + 160.0f, 160.0f, 20.0f, app.currentColor
    );
}

} // namespace UIRenderer