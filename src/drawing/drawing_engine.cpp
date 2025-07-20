#include "../../include/drawing_engine.h"
#include "../../include/app_state.h"

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

} // namespace DrawingEngine