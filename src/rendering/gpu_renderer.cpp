#include "../../include/gpu_renderer.h"
#include "../../include/app_state.h"
#include <algorithm>

namespace GPURenderer {

// Static member definitions
GPUContext GPURenderingEngine::context = {};

bool GPURenderingEngine::Initialize(HWND hwnd) {
    HRESULT hr = S_OK;
    
    // Create D2D1 factory
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &context.d2dFactory
    );
    if (FAILED(hr)) return false;
    
    // Create DirectWrite factory
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&context.writeFactory)
    );
    if (FAILED(hr)) return false;
    
    // Create WIC factory for image operations
    hr = CoCreateInstance(
        CLSID_WICImagingFactory2,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&context.wicFactory)
    );
    if (FAILED(hr)) return false;
    
    // Create device-dependent resources
    if (!CreateDeviceResources(hwnd)) return false;
    
    // Create default text format
    hr = context.writeFactory->CreateTextFormat(
        L"Segoe UI",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        12.0f,
        L"",
        &context.defaultTextFormat
    );
    if (FAILED(hr)) return false;
    
    CreateBrushes();
    CreateStrokeStyles();
    
    context.initialized = true;
    return true;
}

bool GPURenderingEngine::CreateDeviceResources(HWND hwnd) {
    HRESULT hr = S_OK;
    
    if (context.renderTarget) return true; // Already created
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    
    D2D1_SIZE_U size = D2D1::SizeU(
        clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top
    );
    
    // Create hardware-accelerated render target
    hr = context.d2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f,
            D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT
        ),
        D2D1::HwndRenderTargetProperties(hwnd, size),
        &context.renderTarget
    );
    
    return SUCCEEDED(hr);
}

void GPURenderingEngine::CreateBrushes() {
    if (!context.renderTarget) return;
    
    // Create common brushes
    context.renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &context.blackBrush
    );
    
    context.renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &context.whiteBrush
    );
    
    context.renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0.95f, 0.95f, 0.95f, 1.0f), // Light gray
        &context.backgroundBrush
    );
    
    context.renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &context.dynamicBrush
    );
}

HRESULT GPURenderingEngine::CreateStrokeStyles() {
    HRESULT hr = S_OK;
    
    // Create default stroke style
    hr = context.d2dFactory->CreateStrokeStyle(
        D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_ROUND,
            D2D1_CAP_STYLE_ROUND,
            D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_ROUND,
            10.0f,
            D2D1_DASH_STYLE_SOLID,
            0.0f
        ),
        nullptr,
        0,
        &context.defaultStroke
    );
    
    // Create dashed stroke style
    float dashes[] = {5.0f, 5.0f};
    hr = context.d2dFactory->CreateStrokeStyle(
        D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_ROUND,
            D2D1_CAP_STYLE_ROUND,
            D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_ROUND,
            10.0f,
            D2D1_DASH_STYLE_CUSTOM,
            0.0f
        ),
        dashes,
        ARRAYSIZE(dashes),
        &context.dashedStroke
    );
    
    return hr;
}

void GPURenderingEngine::BeginDraw() {
    if (context.renderTarget) {
        context.renderTarget->BeginDraw();
    }
}

void GPURenderingEngine::EndDraw() {
    if (context.renderTarget) {
        HRESULT hr = context.renderTarget->EndDraw();
        
        if (hr == D2DERR_RECREATE_TARGET) {
            // Device lost, need to recreate resources
            ReleaseBrushes();
            context.renderTarget->Release();
            context.renderTarget = nullptr;
            // Will be recreated on next draw
        }
    }
}

void GPURenderingEngine::Clear(COLORREF color) {
    if (context.renderTarget) {
        D2D1::ColorF d2dColor = ColorFromCOLORREF(color);
        context.renderTarget->Clear(d2dColor);
    }
}

void GPURenderingEngine::DrawLine(float x1, float y1, float x2, float y2, COLORREF color, float strokeWidth) {
    if (!context.renderTarget || !context.dynamicBrush) return;
    
    UpdateDynamicBrush(color);
    
    context.renderTarget->DrawLine(
        D2D1::Point2F(x1, y1),
        D2D1::Point2F(x2, y2),
        context.dynamicBrush,
        strokeWidth,
        context.defaultStroke
    );
}

void GPURenderingEngine::FillRectangle(float x, float y, float width, float height, COLORREF color) {
    if (!context.renderTarget || !context.dynamicBrush) return;
    
    UpdateDynamicBrush(color);
    
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
    context.renderTarget->FillRectangle(rect, context.dynamicBrush);
}

void GPURenderingEngine::DrawRectangle(float x, float y, float width, float height, COLORREF color, float strokeWidth) {
    if (!context.renderTarget || !context.dynamicBrush) return;
    
    UpdateDynamicBrush(color);
    
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
    context.renderTarget->DrawRectangle(rect, context.dynamicBrush, strokeWidth);
}

void GPURenderingEngine::FillEllipse(float centerX, float centerY, float radiusX, float radiusY, COLORREF color) {
    if (!context.renderTarget || !context.dynamicBrush) return;
    
    UpdateDynamicBrush(color);
    
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(centerX, centerY), radiusX, radiusY);
    context.renderTarget->FillEllipse(ellipse, context.dynamicBrush);
}

void GPURenderingEngine::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, COLORREF color, float strokeWidth) {
    if (!context.renderTarget || !context.dynamicBrush) return;
    
    UpdateDynamicBrush(color);
    
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(centerX, centerY), radiusX, radiusY);
    context.renderTarget->DrawEllipse(ellipse, context.dynamicBrush, strokeWidth);
}

void GPURenderingEngine::DrawBrushStroke(const std::vector<D2D1_POINT_2F>& points, COLORREF color, float brushSize) {
    if (!context.renderTarget || !context.dynamicBrush || points.size() < 2) return;
    
    UpdateDynamicBrush(color);
    
    // Create path geometry for smooth brush stroke
    ID2D1PathGeometry* pathGeometry = nullptr;
    ID2D1GeometrySink* geometrySink = nullptr;
    
    HRESULT hr = context.d2dFactory->CreatePathGeometry(&pathGeometry);
    if (SUCCEEDED(hr)) {
        hr = pathGeometry->Open(&geometrySink);
        if (SUCCEEDED(hr)) {
            geometrySink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
            
            // Add bezier curves for smooth strokes
            for (size_t i = 1; i < points.size(); ++i) {
                geometrySink->AddLine(points[i]);
            }
            
            geometrySink->EndFigure(D2D1_FIGURE_END_OPEN);
            hr = geometrySink->Close();
            geometrySink->Release();
            
            if (SUCCEEDED(hr)) {
                context.renderTarget->DrawGeometry(pathGeometry, context.dynamicBrush, brushSize, context.defaultStroke);
            }
        }
        pathGeometry->Release();
    }
}

void GPURenderingEngine::DrawIcon(float x, float y, const char* iconData, int width, int height, COLORREF color, float scale) {
    if (!context.renderTarget || !context.dynamicBrush || !iconData) return;
    
    UpdateDynamicBrush(color);
    
    // Parse icon data and draw as rectangles (much faster than GDI)
    int dataIndex = 0;
    float pixelSize = scale;
    
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // Skip non-pixel characters
            while (iconData[dataIndex] != '0' && iconData[dataIndex] != '1' && iconData[dataIndex] != '\0') {
                dataIndex++;
            }
            
            if (iconData[dataIndex] == '\0') return;
            
            if (iconData[dataIndex] == '1') {
                // Draw pixel as small rectangle - GPU accelerated!
                D2D1_RECT_F pixelRect = D2D1::RectF(
                    x + col * pixelSize,
                    y + row * pixelSize,
                    x + (col + 1) * pixelSize,
                    y + (row + 1) * pixelSize
                );
                context.renderTarget->FillRectangle(pixelRect, context.dynamicBrush);
            }
            dataIndex++;
        }
    }
}

void GPURenderingEngine::DrawText(const WCHAR* text, float x, float y, float width, float height, COLORREF color) {
    if (!context.renderTarget || !context.defaultTextFormat || !context.dynamicBrush) return;
    
    UpdateDynamicBrush(color);
    
    D2D1_RECT_F textRect = D2D1::RectF(x, y, x + width, y + height);
    context.renderTarget->DrawTextW(
        text,
        wcslen(text),
        context.defaultTextFormat,
        textRect,
        context.dynamicBrush
    );
}

D2D1::ColorF GPURenderingEngine::ColorFromCOLORREF(COLORREF color, float alpha) {
    return D2D1::ColorF(
        GetRValue(color) / 255.0f,
        GetGValue(color) / 255.0f,
        GetBValue(color) / 255.0f,
        alpha
    );
}

void GPURenderingEngine::UpdateDynamicBrush(COLORREF color, float alpha) {
    if (context.dynamicBrush) {
        D2D1::ColorF d2dColor = ColorFromCOLORREF(color, alpha);
        context.dynamicBrush->SetColor(d2dColor);
    }
}

void GPURenderingEngine::SetTransform(const D2D1_MATRIX_3X2_F& transform) {
    if (context.renderTarget) {
        context.renderTarget->SetTransform(transform);
    }
}

void GPURenderingEngine::ResetTransform() {
    if (context.renderTarget) {
        context.renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    }
}

GPUContext& GPURenderingEngine::GetContext() {
    return context;
}

void GPURenderingEngine::ReleaseBrushes() {
    if (context.blackBrush) { context.blackBrush->Release(); context.blackBrush = nullptr; }
    if (context.whiteBrush) { context.whiteBrush->Release(); context.whiteBrush = nullptr; }
    if (context.backgroundBrush) { context.backgroundBrush->Release(); context.backgroundBrush = nullptr; }
    if (context.dynamicBrush) { context.dynamicBrush->Release(); context.dynamicBrush = nullptr; }
}

void GPURenderingEngine::Shutdown() {
    ReleaseBrushes();
    
    if (context.defaultStroke) { context.defaultStroke->Release(); context.defaultStroke = nullptr; }
    if (context.dashedStroke) { context.dashedStroke->Release(); context.dashedStroke = nullptr; }
    if (context.defaultTextFormat) { context.defaultTextFormat->Release(); context.defaultTextFormat = nullptr; }
    if (context.renderTarget) { context.renderTarget->Release(); context.renderTarget = nullptr; }
    if (context.writeFactory) { context.writeFactory->Release(); context.writeFactory = nullptr; }
    if (context.wicFactory) { context.wicFactory->Release(); context.wicFactory = nullptr; }
    if (context.d2dFactory) { context.d2dFactory->Release(); context.d2dFactory = nullptr; }
    
    context.initialized = false;
}

} // namespace GPURenderer