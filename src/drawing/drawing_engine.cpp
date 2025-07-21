#include "../../include/drawing_engine.h"
#include "../../include/app_state.h"
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <cctype>

namespace DrawingEngine {

COLORREF HSVtoRGB(float h, float s, float v) 
{
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    
    float r, g, b;
    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return RGB((int)((r + m) * 255), (int)((g + m) * 255), (int)((b + m) * 255));
}

void StartDrawing(int x, int y) 
{
    AppState& app = AppState::Instance();
    
    app.isDrawing = true;
    app.drawStartX = x;
    app.drawStartY = y;
    app.drawCurrentX = x;
    app.drawCurrentY = y;
    
    if (app.currentTool == TOOL_BRUSH) {
        DrawPoint point = {x, y, app.currentColor, true, app.brushSize, app.currentTool};
        app.drawingPoints.push_back(point);
    } else if (app.currentTool == TOOL_ERASER) {
        EraseAtPoint(x, y);
    } else {
        // For shapes (rectangle, circle, line), we'll draw preview during drag
        app.hasPreview = true;
    }
}

void ContinueDrawing(int x, int y) 
{
    AppState& app = AppState::Instance();
    
    if (app.isDrawing) {
        app.drawCurrentX = x;
        app.drawCurrentY = y;
        
        if (app.currentTool == TOOL_BRUSH) {
            DrawPoint point = {x, y, app.currentColor, false, app.brushSize, app.currentTool};
            app.drawingPoints.push_back(point);
        } else if (app.currentTool == TOOL_ERASER) {
            EraseAtPoint(x, y);
        }
        // For shapes, we just update preview coordinates
    }
}

void EndDrawing() 
{
    AppState& app = AppState::Instance();
    
    if (app.isDrawing) {
        app.isDrawing = false;
        app.hasPreview = false;
        
        // Finalize shape drawing
        if (app.currentTool == TOOL_RECTANGLE) {
            DrawRectangle(app.drawStartX, app.drawStartY, app.drawCurrentX, app.drawCurrentY);
        } else if (app.currentTool == TOOL_CIRCLE) {
            int radius = (int)sqrt(pow(app.drawCurrentX - app.drawStartX, 2) + pow(app.drawCurrentY - app.drawStartY, 2));
            DrawCircle(app.drawStartX, app.drawStartY, radius);
        } else if (app.currentTool == TOOL_LINE) {
            DrawLine(app.drawStartX, app.drawStartY, app.drawCurrentX, app.drawCurrentY);
        }
        
        SaveState();
    }
}

void ClearCanvas() 
{
    AppState& app = AppState::Instance();
    
    app.drawingPoints.clear();
    SaveState();
}

void SaveState() 
{
    AppState& app = AppState::Instance();
    
    UndoState state;
    state.points = app.drawingPoints;
    app.undoStack.push_back(state);
    
    // Clear redo stack when new action is performed
    app.redoStack.clear();
    
    // Limit undo stack size
    if (app.undoStack.size() > 50) {
        app.undoStack.erase(app.undoStack.begin());
    }
}

bool Undo() 
{
    AppState& app = AppState::Instance();
    
    if (!app.undoStack.empty()) {
        // Save current state to redo stack
        UndoState currentState;
        currentState.points = app.drawingPoints;
        app.redoStack.push_back(currentState);
        
        // Restore previous state
        UndoState previousState = app.undoStack.back();
        app.undoStack.pop_back();
        app.drawingPoints = previousState.points;
        
        return true;
    }
    return false;
}

bool Redo() 
{
    AppState& app = AppState::Instance();
    
    if (!app.redoStack.empty()) {
        // Save current state to undo stack
        SaveState();
        
        // Restore next state
        UndoState nextState = app.redoStack.back();
        app.redoStack.pop_back();
        app.drawingPoints = nextState.points;
        
        return true;
    }
    return false;
}

void SetTool(ToolType tool) 
{
    AppState& app = AppState::Instance();
    app.currentTool = tool;
}

void SetBrushSize(int size) 
{
    AppState& app = AppState::Instance();
    if (size >= 1 && size <= 20) {
        app.brushSize = size;
    }
}

void SetColor(COLORREF color) 
{
    AppState& app = AppState::Instance();
    app.currentColor = color;
}

void DrawRectangle(int startX, int startY, int endX, int endY) 
{
    AppState& app = AppState::Instance();
    
    // Draw rectangle outline using line segments
    int left = std::min(startX, endX);
    int right = std::max(startX, endX);
    int top = std::min(startY, endY);
    int bottom = std::max(startY, endY);
    
    // Top line
    for (int x = left; x <= right; x++) {
        DrawPoint point = {x, top, app.currentColor, x == left, app.brushSize, TOOL_RECTANGLE};
        app.drawingPoints.push_back(point);
    }
    // Bottom line
    for (int x = left; x <= right; x++) {
        DrawPoint point = {x, bottom, app.currentColor, x == left, app.brushSize, TOOL_RECTANGLE};
        app.drawingPoints.push_back(point);
    }
    // Left line
    for (int y = top + 1; y < bottom; y++) {
        DrawPoint point = {left, y, app.currentColor, y == top + 1, app.brushSize, TOOL_RECTANGLE};
        app.drawingPoints.push_back(point);
    }
    // Right line
    for (int y = top + 1; y < bottom; y++) {
        DrawPoint point = {right, y, app.currentColor, y == top + 1, app.brushSize, TOOL_RECTANGLE};
        app.drawingPoints.push_back(point);
    }
}

void DrawCircle(int centerX, int centerY, int radius) 
{
    AppState& app = AppState::Instance();
    
    // Draw circle outline using Bresenham's circle algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    bool isFirst = true;
    
    while (y >= x) {
        // Draw 8 points for each calculated point
        std::vector<std::pair<int, int>> points = {
            {centerX + x, centerY + y}, {centerX - x, centerY + y},
            {centerX + x, centerY - y}, {centerX - x, centerY - y},
            {centerX + y, centerY + x}, {centerX - y, centerY + x},
            {centerX + y, centerY - x}, {centerX - y, centerY - x}
        };
        
        for (auto& p : points) {
            DrawPoint point = {p.first, p.second, app.currentColor, isFirst, app.brushSize, TOOL_CIRCLE};
            app.drawingPoints.push_back(point);
            isFirst = false;
        }
        
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

void DrawLine(int startX, int startY, int endX, int endY) 
{
    AppState& app = AppState::Instance();
    
    // Draw line using Bresenham's line algorithm
    int dx = std::abs(endX - startX);
    int dy = std::abs(endY - startY);
    int sx = (startX < endX) ? 1 : -1;
    int sy = (startY < endY) ? 1 : -1;
    int err = dx - dy;
    
    int x = startX;
    int y = startY;
    bool isFirst = true;
    
    while (true) {
        DrawPoint point = {x, y, app.currentColor, isFirst, app.brushSize, TOOL_LINE};
        app.drawingPoints.push_back(point);
        isFirst = false;
        
        if (x == endX && y == endY) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void EraseAtPoint(int x, int y) 
{
    AppState& app = AppState::Instance();
    
    // Remove points within eraser radius
    int eraseRadius = app.brushSize;
    
    auto it = app.drawingPoints.begin();
    while (it != app.drawingPoints.end()) {
        int dx = it->x - x;
        int dy = it->y - y;
        int distance = (int)sqrt(dx * dx + dy * dy);
        
        if (distance <= eraseRadius) {
            it = app.drawingPoints.erase(it);
        } else {
            ++it;
        }
    }
}

COLORREF PickColorAt(HDC hdc, int x, int y) 
{
    return GetPixel(hdc, x, y);
}

bool SaveDrawing(const std::string& filename)
{
    AppState& app = AppState::Instance();
    
    std::FILE* file = std::fopen(filename.c_str(), "wb");
    if (!file) {
        return false;
    }
    
    // Write file header
    const char header[] = "MPSP"; // Modern Paint Studio Pro
    const uint32_t version = 1;
    std::fwrite(header, 1, 4, file);
    std::fwrite(&version, sizeof(uint32_t), 1, file);
    
    // Write number of points
    uint32_t pointCount = static_cast<uint32_t>(app.drawingPoints.size());
    std::fwrite(&pointCount, sizeof(uint32_t), 1, file);
    
    // Write drawing points
    for (const DrawPoint& point : app.drawingPoints) {
        std::fwrite(&point.x, sizeof(int), 1, file);
        std::fwrite(&point.y, sizeof(int), 1, file);
        std::fwrite(&point.color, sizeof(COLORREF), 1, file);
        std::fwrite(&point.isStart, sizeof(bool), 1, file);
        std::fwrite(&point.brushSize, sizeof(int), 1, file);
        std::fwrite(&point.toolType, sizeof(ToolType), 1, file);
    }
    
    std::fclose(file);
    return true;
}

bool LoadDrawing(const std::string& filename)
{
    std::FILE* file = std::fopen(filename.c_str(), "rb");
    if (!file) {
        return false;
    }
    
    // Read and verify header
    char header[5] = {0};
    uint32_t version;
    if (std::fread(header, 1, 4, file) != 4 ||
        std::strcmp(header, "MPSP") != 0 ||
        std::fread(&version, sizeof(uint32_t), 1, file) != 1 ||
        version != 1) {
        std::fclose(file);
        return false;
    }
    
    // Read number of points
    uint32_t pointCount;
    if (std::fread(&pointCount, sizeof(uint32_t), 1, file) != 1) {
        std::fclose(file);
        return false;
    }
    
    // Clear current drawing
    AppState& app = AppState::Instance();
    app.drawingPoints.clear();
    
    // Read drawing points
    for (uint32_t i = 0; i < pointCount; i++) {
        DrawPoint point;
        if (std::fread(&point.x, sizeof(int), 1, file) != 1 ||
            std::fread(&point.y, sizeof(int), 1, file) != 1 ||
            std::fread(&point.color, sizeof(COLORREF), 1, file) != 1 ||
            std::fread(&point.isStart, sizeof(bool), 1, file) != 1 ||
            std::fread(&point.brushSize, sizeof(int), 1, file) != 1 ||
            std::fread(&point.toolType, sizeof(ToolType), 1, file) != 1) {
            std::fclose(file);
            return false;
        }
        app.drawingPoints.push_back(point);
    }
    
    std::fclose(file);
    SaveState(); // Add to undo stack
    return true;
}

bool ExportAsBitmap(const std::string& filename, int width, int height)
{
    AppState& app = AppState::Instance();
    
    // Create a bitmap
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    
    // Fill with white background
    RECT rect = {0, 0, width, height};
    FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // Draw strokes properly by connecting points
    if (!app.drawingPoints.empty()) {
        HPEN currentPen = NULL;
        HBRUSH currentBrush = NULL;
        COLORREF currentColor = RGB(0, 0, 0);
        int currentSize = 1;
        
        for (size_t i = 0; i < app.drawingPoints.size(); i++) {
            const DrawPoint& point = app.drawingPoints[i];
            
            // Skip points outside canvas bounds
            if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height) {
                continue;
            }
            
            // Create new pen/brush if color or size changed
            if (currentColor != point.color || currentSize != point.brushSize) {
                if (currentPen) DeleteObject(currentPen);
                if (currentBrush) DeleteObject(currentBrush);
                
                currentColor = point.color;
                currentSize = point.brushSize;
                currentPen = CreatePen(PS_SOLID, currentSize, currentColor);
                currentBrush = CreateSolidBrush(currentColor);
            }
            
            // Handle different drawing tools
            if (point.toolType == TOOL_BRUSH) {
                SelectObject(hdcMem, currentPen);
                
                if (point.isStart) {
                    // Start a new stroke
                    MoveToEx(hdcMem, point.x, point.y, NULL);
                } else {
                    // Continue the stroke
                    LineTo(hdcMem, point.x, point.y);
                }
                
                // Also draw a circle at each point for brush texture
                SelectObject(hdcMem, currentBrush);
                int radius = currentSize / 2;
                Ellipse(hdcMem, 
                       point.x - radius, point.y - radius, 
                       point.x + radius, point.y + radius);
            }
            else if (point.toolType == TOOL_ERASER) {
                // Draw white circles for eraser
                HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdcMem, whiteBrush);
                int radius = currentSize / 2;
                Ellipse(hdcMem, 
                       point.x - radius, point.y - radius, 
                       point.x + radius, point.y + radius);
                DeleteObject(whiteBrush);
            }
            else {
                // For shapes (rectangle, circle, line), just draw points for now
                // The actual shape reconstruction would require more complex logic
                SelectObject(hdcMem, currentBrush);
                int radius = std::max(1, currentSize / 2);
                Ellipse(hdcMem, 
                       point.x - radius, point.y - radius, 
                       point.x + radius, point.y + radius);
            }
        }
        
        // Cleanup
        if (currentPen) DeleteObject(currentPen);
        if (currentBrush) DeleteObject(currentBrush);
    }
    
    // Save bitmap using GDI+
    SelectObject(hdcMem, hOldBitmap);
    
    // Convert filename to wide string
    std::wstring wFilename(filename.begin(), filename.end());
    
    Bitmap bitmap(hBitmap, NULL);
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    Status status = bitmap.Save(wFilename.c_str(), &pngClsid, NULL);
    
    // Cleanup
    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hdcScreen);
    
    return status == Ok;
}

// Helper function to get encoder CLSID for bitmap export
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;
    UINT size = 0;
    
    GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    
    GetImageEncoders(num, size, pImageCodecInfo);
    
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    
    free(pImageCodecInfo);
    return -1;
}

std::string EnsureFileExtension(const std::string& filename, const std::string& extension)
{
    // Check if filename already has the extension (case insensitive)
    std::string lowerFilename = filename;
    std::string lowerExtension = extension;
    
    // Convert to lowercase for comparison
    std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
    std::transform(lowerExtension.begin(), lowerExtension.end(), lowerExtension.begin(), ::tolower);
    
    // Check if it already ends with the extension
    if (lowerFilename.length() >= lowerExtension.length() &&
        lowerFilename.substr(lowerFilename.length() - lowerExtension.length()) == lowerExtension) {
        return filename; // Already has extension
    }
    
    return filename + extension;
}

} // namespace DrawingEngine