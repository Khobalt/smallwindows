#ifndef GPU_RENDERER_H
#define GPU_RENDERER_H

#include "types.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

// GPU-accelerated rendering system for Modern Paint Studio Pro
namespace GPURenderer {

// GPU rendering context
struct GPUContext {
    ID2D1Factory* d2dFactory;
    ID2D1HwndRenderTarget* renderTarget;
    IDWriteFactory* writeFactory;
    IDWriteTextFormat* defaultTextFormat;
    IWICImagingFactory* wicFactory;
    
    // Brushes for common operations
    ID2D1SolidColorBrush* blackBrush;
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* backgroundBrush;
    ID2D1SolidColorBrush* dynamicBrush; // For changing colors
    
    // Stroke styles
    ID2D1StrokeStyle* defaultStroke;
    ID2D1StrokeStyle* dashedStroke;
    
    bool initialized;
};

// GPU rendering interface
class GPURenderingEngine {
public:
    static bool Initialize(HWND hwnd);
    static void Shutdown();
    static GPUContext& GetContext();
    
    // Core rendering
    static void BeginDraw();
    static void EndDraw();
    static void Clear(COLORREF color);
    static void Present();
    
    // Drawing primitives (GPU-accelerated)
    static void DrawLine(float x1, float y1, float x2, float y2, COLORREF color, float strokeWidth);
    static void DrawRectangle(float x, float y, float width, float height, COLORREF color, float strokeWidth);
    static void FillRectangle(float x, float y, float width, float height, COLORREF color);
    static void DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, COLORREF color, float strokeWidth);
    static void FillEllipse(float centerX, float centerY, float radiusX, float radiusY, COLORREF color);
    
    // Advanced drawing
    static void DrawBrushStroke(const std::vector<D2D1_POINT_2F>& points, COLORREF color, float brushSize);
    static void DrawBrushPreview(float x, float y, float brushSize);
    static void DrawIcon(float x, float y, const char* iconData, int width, int height, COLORREF color, float scale = 1.0f);
    static void DrawText(const WCHAR* text, float x, float y, float width, float height, COLORREF color);
    
    // Effects and transformations
    static void SetTransform(const D2D1_MATRIX_3X2_F& transform);
    static void ResetTransform();
    static void SetClipRect(float x, float y, float width, float height);
    static void RemoveClip();
    
    // Texture/bitmap operations
    static ID2D1Bitmap* CreateBitmapFromHDC(HDC hdc, int width, int height);
    static void DrawBitmap(ID2D1Bitmap* bitmap, float x, float y, float opacity = 1.0f);
    
    // Color utilities
    static D2D1::ColorF ColorFromCOLORREF(COLORREF color, float alpha = 1.0f);
    static void UpdateDynamicBrush(COLORREF color, float alpha = 1.0f);
    
    // Performance monitoring
    static void GetPerformanceStats();
    
private:
    static GPUContext context;
    static bool CreateDeviceResources(HWND hwnd);
    static void CreateBrushes();
    static void ReleaseBrushes();
    static HRESULT CreateStrokeStyles();
};

// GPU-accelerated canvas management
class GPUCanvas {
public:
    static bool Initialize(int width, int height);
    static void Resize(int width, int height);
    static void Clear(COLORREF backgroundColor);
    
    // Layer management for GPU
    static void BeginLayer();
    static void EndLayer();
    static void SetLayerOpacity(float opacity);
    
    // Advanced canvas operations
    static void ApplyGaussianBlur(float radius);
    static void ApplyColorMatrix(const D2D_MATRIX_5X4_F& matrix);
    static void SaveToFile(const WCHAR* filename);
    
private:
    static ID2D1BitmapRenderTarget* canvasTarget;
    static ID2D1Bitmap* canvasBitmap;
    static int canvasWidth, canvasHeight;
};

} // namespace GPURenderer

#endif // GPU_RENDERER_H