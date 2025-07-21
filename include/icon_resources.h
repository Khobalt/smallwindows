#ifndef ICON_RESOURCES_H
#define ICON_RESOURCES_H

#include "types.h"

// Icon system for Modern Paint Studio Pro
// 16x16 pixel monochrome icons represented as binary patterns

namespace IconResources {

// Icon data structure
struct Icon {
    int width;
    int height;
    const char* data;
    const char* name;
};

// 16x16 icon patterns (1 = pixel on, 0 = pixel off)
// Each row is represented as a hex byte for compactness

// Brush tool icon (paintbrush shape)
const char BRUSH_ICON_DATA[] = {
    "0000001110000000"  // Row 0
    "0000011111000000"  // Row 1
    "0000111111100000"  // Row 2
    "0001111111110000"  // Row 3
    "0011111111111000"  // Row 4
    "0111111111111100"  // Row 5
    "1111111111111110"  // Row 6
    "1111111111111110"  // Row 7
    "0111111111111100"  // Row 8
    "0011111111111000"  // Row 9
    "0001111111110000"  // Row 10
    "0000111111100000"  // Row 11
    "0000011111000000"  // Row 12
    "0000001110000000"  // Row 13
    "0000000100000000"  // Row 14
    "0000000000000000"  // Row 15
};

// Eraser tool icon (rectangular eraser)
const char ERASER_ICON_DATA[] = {
    "0011111111111100"  // Row 0
    "0111111111111110"  // Row 1
    "1111111111111111"  // Row 2
    "1111111111111111"  // Row 3
    "1111111111111111"  // Row 4
    "1111111111111111"  // Row 5
    "1111111111111111"  // Row 6
    "1111111111111111"  // Row 7
    "1111111111111111"  // Row 8
    "1111111111111111"  // Row 9
    "1111111111111111"  // Row 10
    "1111111111111111"  // Row 11
    "1111111111111111"  // Row 12
    "0111111111111110"  // Row 13
    "0011111111111100"  // Row 14
    "0000000000000000"  // Row 15
};

// Rectangle tool icon
const char RECTANGLE_ICON_DATA[] = {
    "1111111111111111"  // Row 0
    "1000000000000001"  // Row 1
    "1000000000000001"  // Row 2
    "1000000000000001"  // Row 3
    "1000000000000001"  // Row 4
    "1000000000000001"  // Row 5
    "1000000000000001"  // Row 6
    "1000000000000001"  // Row 7
    "1000000000000001"  // Row 8
    "1000000000000001"  // Row 9
    "1000000000000001"  // Row 10
    "1000000000000001"  // Row 11
    "1000000000000001"  // Row 12
    "1000000000000001"  // Row 13
    "1111111111111111"  // Row 14
    "0000000000000000"  // Row 15
};

// Circle tool icon
const char CIRCLE_ICON_DATA[] = {
    "0000111111110000"  // Row 0
    "0011111111111100"  // Row 1
    "0111111111111110"  // Row 2
    "1111111111111111"  // Row 3
    "1111000000001111"  // Row 4
    "1110000000000111"  // Row 5
    "1110000000000111"  // Row 6
    "1110000000000111"  // Row 7
    "1110000000000111"  // Row 8
    "1110000000000111"  // Row 9
    "1110000000000111"  // Row 10
    "1111000000001111"  // Row 11
    "1111111111111111"  // Row 12
    "0111111111111110"  // Row 13
    "0011111111111100"  // Row 14
    "0000111111110000"  // Row 15
};

// Line tool icon
const char LINE_ICON_DATA[] = {
    "1000000000000000"  // Row 0
    "1100000000000000"  // Row 1
    "0110000000000000"  // Row 2
    "0011000000000000"  // Row 3
    "0001100000000000"  // Row 4
    "0000110000000000"  // Row 5
    "0000011000000000"  // Row 6
    "0000001100000000"  // Row 7
    "0000000110000000"  // Row 8
    "0000000011000000"  // Row 9
    "0000000001100000"  // Row 10
    "0000000000110000"  // Row 11
    "0000000000011000"  // Row 12
    "0000000000001100"  // Row 13
    "0000000000000110"  // Row 14
    "0000000000000011"  // Row 15
};

// Color picker tool icon (eyedropper)
const char PICKER_ICON_DATA[] = {
    "0000000000001110"  // Row 0
    "0000000000011111"  // Row 1
    "0000000000111111"  // Row 2
    "0000000001111110"  // Row 3
    "0000000011111100"  // Row 4
    "0000000111111000"  // Row 5
    "0000001111110000"  // Row 6
    "0000011111100000"  // Row 7
    "0000111111000000"  // Row 8
    "0001111110000000"  // Row 9
    "0011111100000000"  // Row 10
    "0111111000000000"  // Row 11
    "1111110000000000"  // Row 12
    "1111100000000000"  // Row 13
    "1111000000000000"  // Row 14
    "1110000000000000"  // Row 15
};

// Icon declarations (defined in icon_renderer.cpp)
extern const Icon BRUSH_ICON;
extern const Icon ERASER_ICON;
extern const Icon RECTANGLE_ICON;
extern const Icon CIRCLE_ICON;
extern const Icon LINE_ICON;
extern const Icon PICKER_ICON;

// Icon array for easy access by tool index
extern const Icon* TOOL_ICONS[];

// Function to render an icon to HDC
void DrawIcon(HDC hdc, int x, int y, const Icon* icon, COLORREF color, int scale = 1);

// Function to get icon by tool type
const Icon* GetToolIcon(ToolType tool);

} // namespace IconResources

#endif // ICON_RESOURCES_H