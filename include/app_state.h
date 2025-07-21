#ifndef APP_STATE_H
#define APP_STATE_H

#include "types.h"

// Global application state
class AppState {
public:
    // Drawing state
    std::vector<DrawPoint> drawingPoints;
    std::vector<UndoState> undoStack;
    std::vector<UndoState> redoStack;
    bool isDrawing = false;
    
    // Temporary drawing state for shapes/preview
    int drawStartX = 0, drawStartY = 0;
    int drawCurrentX = 0, drawCurrentY = 0;
    bool hasPreview = false;
    
    // Current settings
    COLORREF currentColor = RGB(0, 0, 0);
    int brushSize = 5;
    ToolType currentTool = TOOL_BRUSH;
    ThemeType currentTheme = THEME_LIGHT;
    
    // View state
    float zoomLevel = 1.0f;
    int panX = 0, panY = 0;
    bool showGrid = false;
    bool showAdvancedColorPicker = false;
    int pickerX = 400, pickerY = 200;
    
    // UI state for hover effects
    int hoveredTool = -1;  // -1 means no tool is hovered
    bool hoveredAdvancedPicker = false;
    bool hoveredThemeButton = false;
    
    // GDI+ token
    ULONG_PTR gdiplusToken;
    
    // Singleton access
    static AppState& Instance();
    
private:
    AppState() = default;
    static AppState* instance;
};

#endif // APP_STATE_H