#include "../include/types.h"
#include "../include/config.h"
#include "../include/app_state.h"
#include "../include/ui_renderer.h"
#include "../include/drawing_engine.h"
#include "../include/event_handler.h"

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    AppState& app = AppState::Instance();
    
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&app.gdiplusToken, &gdiplusStartupInput, NULL);
    
    WNDCLASSEX wincl;
    HWND hwnd;
    MSG messages;

    // Set properties of the window class.
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    // Register the window class, and if it fails quit the program
    if (!RegisterClassEx(&wincl))
        return 0;

    // Create the window.
    hwnd = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        szClassName,
        L"Modern Paint Studio Pro - Advanced Digital Art Application",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        1200,
        800,
        NULL,
        UIRenderer::CreateMenuBar(),
        hThisInstance,
        NULL);

    // Make the window visible on the screen
    ShowWindow(hwnd, nCmdShow);
    SetForegroundWindow(hwnd);
    UpdateWindow(hwnd);

    // Run the message loop. It will run until GetMessage() returns 0
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    // Cleanup GDI+
    GdiplusShutdown(app.gdiplusToken);

    return messages.wParam;
}