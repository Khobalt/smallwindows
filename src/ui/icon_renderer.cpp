#include "../../include/icon_resources.h"

namespace IconResources {

// Icon definitions
const Icon BRUSH_ICON = {16, 16, BRUSH_ICON_DATA, "Brush"};
const Icon ERASER_ICON = {16, 16, ERASER_ICON_DATA, "Eraser"};  
const Icon RECTANGLE_ICON = {16, 16, RECTANGLE_ICON_DATA, "Rectangle"};
const Icon CIRCLE_ICON = {16, 16, CIRCLE_ICON_DATA, "Circle"};
const Icon LINE_ICON = {16, 16, LINE_ICON_DATA, "Line"};
const Icon PICKER_ICON = {16, 16, PICKER_ICON_DATA, "Picker"};

// Icon array for easy access by tool index
const Icon* TOOL_ICONS[] = {
    &BRUSH_ICON,     // TOOL_BRUSH = 0
    &ERASER_ICON,    // TOOL_ERASER = 1
    &RECTANGLE_ICON, // TOOL_RECTANGLE = 2
    &CIRCLE_ICON,    // TOOL_CIRCLE = 3
    &LINE_ICON,      // TOOL_LINE = 4
    &PICKER_ICON     // TOOL_PICKER = 5
};

void DrawIcon(HDC hdc, int x, int y, const Icon* icon, COLORREF color, int scale) {
    if (!icon || !icon->data) return;
    
    // Create a brush for drawing pixels
    HBRUSH iconBrush = CreateSolidBrush(color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, iconBrush);
    
    // Parse the icon data and draw pixels
    const char* data = icon->data;
    int dataIndex = 0;
    
    for (int row = 0; row < icon->height; row++) {
        for (int col = 0; col < icon->width; col++) {
            // Skip newlines and other non-pixel characters
            while (data[dataIndex] != '0' && data[dataIndex] != '1' && data[dataIndex] != '\0') {
                dataIndex++;
            }
            
            if (data[dataIndex] == '\0') break;
            
            if (data[dataIndex] == '1') {
                // Draw pixel(s) based on scale
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        SetPixel(hdc, x + col * scale + sx, y + row * scale + sy, color);
                    }
                }
            }
            dataIndex++;
        }
    }
    
    SelectObject(hdc, oldBrush);
    DeleteObject(iconBrush);
}

const Icon* GetToolIcon(ToolType tool) {
    if (tool >= 0 && tool < 6) {
        return TOOL_ICONS[tool];
    }
    return &BRUSH_ICON; // Default fallback
}

} // namespace IconResources