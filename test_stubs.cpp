// Test stubs and extracted functions from helloworld.cpp for testing

#include <windows.h>
#include <cmath>

// Extract the HSVtoRGB function for testing
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

// Extract the color palette for testing
COLORREF colorPalette[] = {
    RGB(0, 0, 0), RGB(128, 128, 128), RGB(255, 0, 0), RGB(255, 128, 0),
    RGB(255, 255, 0), RGB(0, 255, 0), RGB(0, 255, 255), RGB(0, 0, 255),
    RGB(255, 0, 255), RGB(255, 255, 255), RGB(128, 0, 0), RGB(0, 128, 0),
    RGB(0, 0, 128), RGB(128, 0, 128), RGB(128, 128, 0), RGB(192, 192, 192)
};

// Test data structures
struct DrawPoint {
    int x, y;
    COLORREF color;
    bool isStart;
    int brushSize;
};

struct UndoState {
    // In a real implementation this would contain drawing points
    int pointCount;
};

enum ToolType {
    TOOL_BRUSH,
    TOOL_ERASER,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_LINE,
    TOOL_PICKER
};

enum ThemeType {
    THEME_LIGHT,
    THEME_DARK
};

// Menu IDs (extracted from main app)
#define IDM_FILE_NEW        1001
#define IDM_FILE_OPEN       1002
#define IDM_FILE_SAVE       1003
#define IDM_FILE_SAVEAS     1004
#define IDM_FILE_EXIT       1005
#define IDM_EDIT_UNDO       1006
#define IDM_EDIT_REDO       1007
#define IDM_EDIT_CLEAR      1008
#define IDM_VIEW_ZOOM_IN    1009
#define IDM_VIEW_ZOOM_OUT   1010
#define IDM_VIEW_ZOOM_FIT   1011
#define IDM_VIEW_GRID       1012
#define IDM_VIEW_THEME      1013
#define IDM_TOOLS_BRUSH     1014
#define IDM_TOOLS_ERASER    1015
#define IDM_TOOLS_RECT      1016
#define IDM_TOOLS_CIRCLE    1017
#define IDM_TOOLS_LINE      1018
#define IDM_HELP_ABOUT      1019

// Test helper functions
bool IsValidToolType(int tool) {
    return tool >= TOOL_BRUSH && tool <= TOOL_PICKER;
}

bool IsValidBrushSize(int size) {
    return size >= 1 && size <= 20;
}

bool IsValidZoomLevel(float zoom) {
    return zoom >= 0.2f && zoom <= 5.0f;
}

bool IsPointInToolbar(int x, int y, int toolbarHeight) {
    return y >= 0 && y < toolbarHeight;
}

bool IsPointInCanvas(int x, int y, int windowWidth, int windowHeight, int toolbarHeight, int statusbarHeight) {
    return y >= toolbarHeight && y < (windowHeight - statusbarHeight) && x >= 0 && x < windowWidth;
}

int CalculateBrushSizeFromSlider(int x, int sliderStart, int sliderEnd) {
    if (x < sliderStart) x = sliderStart;
    if (x > sliderEnd) x = sliderEnd;
    return 1 + ((x - sliderStart) * 19) / (sliderEnd - sliderStart);
}

int CalculateToolFromPosition(int x, int toolWidth) {
    return x / toolWidth;
}

int CalculateColorFromPosition(int x, int paletteStart, int colorWidth) {
    if (x < paletteStart) return -1;
    return (x - paletteStart) / colorWidth;
}