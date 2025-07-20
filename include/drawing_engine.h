#ifndef DRAWING_ENGINE_H
#define DRAWING_ENGINE_H

#include "types.h"

// Drawing system functions
namespace DrawingEngine {
    // Color utilities
    COLORREF HSVtoRGB(float h, float s, float v);
    
    // Drawing operations
    void StartDrawing(int x, int y);
    void ContinueDrawing(int x, int y);
    void EndDrawing();
    void ClearCanvas();
    
    // Tool-specific operations
    void DrawRectangle(int startX, int startY, int endX, int endY);
    void DrawCircle(int centerX, int centerY, int radius);
    void DrawLine(int startX, int startY, int endX, int endY);
    void EraseAtPoint(int x, int y);
    COLORREF PickColorAt(HDC hdc, int x, int y);
    
    // Undo/Redo system
    void SaveState();
    bool Undo();
    bool Redo();
    
    // Tool operations
    void SetTool(ToolType tool);
    void SetBrushSize(int size);
    void SetColor(COLORREF color);
}

#endif // DRAWING_ENGINE_H