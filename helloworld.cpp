#include <windows.h>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

char const *szClassName = "MyWindowClass";

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    WNDCLASSEX wincl;
    HWND hwnd;
    MSG messages;

    // Set properties of the window class.
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    // Load icons and cursors for the window.
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    // Register the window class.
    if (!RegisterClassEx(&wincl))
        return 0;

    // Create the window.
    hwnd = CreateWindowEx(
        0,
        szClassName,
        "Lee's Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        544,
        375,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    // Display the window on the screen.
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop: Retrieve and dispatch messages until a WM_QUIT message is received.
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // The text to be displayed.
            const char* text = "All work and no play makes Jack a dull boy";
            
            // Loop to display the text multiple times.
            for (int y = 0; y < 300; y += 20) {
                TextOut(hdc, 10, y, text, strlen(text));
            }

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
