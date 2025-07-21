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

} // namespace UIRenderer