#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "types.h"

// Event handling functions
namespace EventHandler {
    // Mouse events
    void OnLeftButtonDown(HWND hwnd, int x, int y);
    void OnMouseMove(HWND hwnd, WPARAM wParam, int x, int y);
    void OnLeftButtonUp(HWND hwnd, int x, int y);
    void OnRightButtonDown(HWND hwnd, int x, int y);
    void OnMouseWheel(HWND hwnd, WPARAM wParam, LPARAM lParam);
    
    // Keyboard events
    void OnKeyDown(HWND hwnd, WPARAM wParam);
    
    // Menu events
    void OnCommand(HWND hwnd, WPARAM wParam);
    
    // Window events
    void OnPaint(HWND hwnd);
    void OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam);
}

#endif // EVENT_HANDLER_H