#ifndef TYPES_H
#define TYPES_H

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <vector>
#include <cstdio>
#include <string>
#include <cmath>
#include <algorithm>
#include <commdlg.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comdlg32.lib")

using namespace Gdiplus;

// Forward declarations
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Application constants
extern const WCHAR szClassName[];

// Tool types
enum ToolType {
    TOOL_BRUSH,
    TOOL_ERASER,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_LINE,
    TOOL_PICKER
};

// Structure to store drawing points
struct DrawPoint {
    int x, y;
    COLORREF color;
    bool isStart;
    int brushSize;
    ToolType toolType;  // Track what tool created this point
};

// Structure for undo system
struct UndoState {
    std::vector<DrawPoint> points;
};

// Theme types
enum ThemeType {
    THEME_LIGHT,
    THEME_DARK
};

#endif // TYPES_H