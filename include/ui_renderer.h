#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "types.h"

// UI Rendering functions
namespace UIRenderer {
    void DrawToolbar(HDC hdc, RECT clientRect);
    void DrawToolbarGPU(RECT clientRect);  // GPU-accelerated version
    void DrawToolbarSoftware(HDC hdc, RECT clientRect);  // Software fallback
    void DrawStatusBar(HDC hdc, RECT clientRect);
    void DrawStatusBarGPU(RECT clientRect);  // GPU-accelerated version
    void DrawAdvancedColorPicker(HDC hdc);
    void DrawAdvancedColorPickerGPU(RECT clientRect);  // GPU-accelerated version
    HMENU CreateMenuBar();
}

#endif // UI_RENDERER_H