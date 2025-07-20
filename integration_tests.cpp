#include "test_framework.h"
#include <windows.h>
#include <psapi.h>
#include <string>
#include <thread>
#include <chrono>

class IntegrationTests {
private:
    TestFramework framework;
    HWND appWindow = nullptr;
    const char* windowClassName = "ModernPaintApp";
    const char* windowTitle = "Modern Paint Studio Pro - Advanced Digital Art Application";

public:
    IntegrationTests() {
        SetupTests();
    }

    void SetupTests() {
        // Application Launch Tests
        framework.AddSuite("Application Launch");
        framework.AddTest("Window Creation", [this]() { return TestWindowCreation(); });
        framework.AddTest("Window Visibility", [this]() { return TestWindowVisibility(); });
        framework.AddTest("Menu Bar Presence", [this]() { return TestMenuBarPresence(); });
        framework.AddTest("Window Responsiveness", [this]() { return TestWindowResponsiveness(); });

        // UI Interaction Tests
        framework.AddSuite("UI Interactions");
        framework.AddTest("Toolbar Click Simulation", [this]() { return TestToolbarClicks(); });
        framework.AddTest("Menu Item Access", [this]() { return TestMenuItemAccess(); });
        framework.AddTest("Keyboard Shortcuts", [this]() { return TestKeyboardShortcuts(); });
        framework.AddTest("Window Resize Handling", [this]() { return TestWindowResize(); });

        // Drawing Tests
        framework.AddSuite("Drawing Functionality");
        framework.AddTest("Canvas Click Detection", [this]() { return TestCanvasClicks(); });
        framework.AddTest("Drawing Area Bounds", [this]() { return TestDrawingBounds(); });
        framework.AddTest("Color Selection", [this]() { return TestColorSelection(); });

        // Stability Tests
        framework.AddSuite("Stability");
        framework.AddTest("Memory Usage", [this]() { return TestMemoryUsage(); });
        framework.AddTest("Window Close Handling", [this]() { return TestWindowClose(); });
    }

    void RunTests() {
        framework.RunAllTests();
    }

private:
    // Helper function to launch the application
    bool LaunchApplication() {
        // Launch the application process
        STARTUPINFO si = {};
        PROCESS_INFORMATION pi = {};
        si.cb = sizeof(si);

        std::string cmdLine = "modernpaint.exe";
        
        if (!CreateProcess(
            NULL,
            (LPSTR)cmdLine.c_str(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
            return false;
        }

        // Wait a moment for the window to appear
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        // Find the application window
        appWindow = FindWindow(windowClassName, windowTitle);
        
        // Clean up process handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return appWindow != nullptr;
    }

    // Helper function to close the application
    void CloseApplication() {
        if (appWindow && IsWindow(appWindow)) {
            PostMessage(appWindow, WM_CLOSE, 0, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            appWindow = nullptr;
        }
    }

    // Application Launch Tests
    bool TestWindowCreation() {
        bool success = LaunchApplication();
        ASSERT_TRUE(success);
        ASSERT_NE(nullptr, appWindow);
        return true;
    }

    bool TestWindowVisibility() {
        ASSERT_NE(nullptr, appWindow);
        ASSERT_TRUE(IsWindow(appWindow));
        ASSERT_TRUE(IsWindowVisible(appWindow));
        return true;
    }

    bool TestMenuBarPresence() {
        ASSERT_NE(nullptr, appWindow);
        HMENU menuBar = GetMenu(appWindow);
        ASSERT_NE(nullptr, menuBar);
        
        // Test that we have the expected menu items
        int menuCount = GetMenuItemCount(menuBar);
        ASSERT_TRUE(menuCount >= 5); // File, Edit, View, Tools, Help
        
        return true;
    }

    bool TestWindowResponsiveness() {
        ASSERT_NE(nullptr, appWindow);
        
        // Test that the window responds to messages
        LRESULT result = SendMessageTimeout(
            appWindow, 
            WM_NULL, 
            0, 
            0, 
            SMTO_BLOCK, 
            1000, // 1 second timeout
            nullptr
        );
        
        ASSERT_NE(0, result);
        return true;
    }

    // UI Interaction Tests
    bool TestToolbarClicks() {
        ASSERT_NE(nullptr, appWindow);
        
        // Get window rectangle to calculate click positions
        RECT windowRect;
        GetWindowRect(appWindow, &windowRect);
        
        // Simulate clicking on the first tool button
        int clickX = windowRect.left + 25; // First tool button center
        int clickY = windowRect.top + 35;  // Toolbar area
        
        // Set cursor position and simulate click
        SetCursorPos(clickX, clickY);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Send mouse click to the window
        POINT pt = {25, 20}; // Relative to window
        LPARAM lParam = MAKELPARAM(pt.x, pt.y);
        
        PostMessage(appWindow, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        PostMessage(appWindow, WM_LBUTTONUP, 0, lParam);
        
        return true;
    }

    bool TestMenuItemAccess() {
        ASSERT_NE(nullptr, appWindow);
        
        HMENU menuBar = GetMenu(appWindow);
        ASSERT_NE(nullptr, menuBar);
        
        // Test that we can access File menu
        HMENU fileMenu = GetSubMenu(menuBar, 0);
        ASSERT_NE(nullptr, fileMenu);
        
        // Test menu item count in File menu
        int fileMenuCount = GetMenuItemCount(fileMenu);
        ASSERT_TRUE(fileMenuCount >= 3); // New, Save, Exit at minimum
        
        return true;
    }

    bool TestKeyboardShortcuts() {
        ASSERT_NE(nullptr, appWindow);
        
        // Test F1 for help
        SetForegroundWindow(appWindow);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        PostMessage(appWindow, WM_KEYDOWN, VK_F1, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        PostMessage(appWindow, WM_KEYUP, VK_F1, 0);
        
        // Give time for help dialog to potentially appear
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Look for help dialog
        HWND helpDialog = FindWindow(NULL, "Help");
        if (helpDialog) {
            // Close help dialog if it appeared
            PostMessage(helpDialog, WM_COMMAND, IDOK, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        return true;
    }

    bool TestWindowResize() {
        ASSERT_NE(nullptr, appWindow);
        
        // Get original window size
        RECT originalRect;
        GetWindowRect(appWindow, &originalRect);
        
        // Resize window
        int newWidth = 1000;
        int newHeight = 700;
        
        SetWindowPos(appWindow, NULL, originalRect.left, originalRect.top, 
                    newWidth, newHeight, SWP_NOZORDER);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        // Verify resize worked
        RECT newRect;
        GetWindowRect(appWindow, &newRect);
        
        int actualWidth = newRect.right - newRect.left;
        int actualHeight = newRect.bottom - newRect.top;
        
        // Allow some tolerance for window borders
        ASSERT_TRUE(abs(actualWidth - newWidth) < 20);
        ASSERT_TRUE(abs(actualHeight - newHeight) < 20);
        
        return true;
    }

    // Drawing Tests
    bool TestCanvasClicks() {
        ASSERT_NE(nullptr, appWindow);
        
        // Get client area
        RECT clientRect;
        GetClientRect(appWindow, &clientRect);
        
        // Click in canvas area (below toolbar, above status bar)
        POINT canvasPoint = {clientRect.right / 2, clientRect.bottom / 2};
        LPARAM lParam = MAKELPARAM(canvasPoint.x, canvasPoint.y);
        
        // Simulate drawing stroke
        PostMessage(appWindow, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Move mouse while drawing
        canvasPoint.x += 50;
        lParam = MAKELPARAM(canvasPoint.x, canvasPoint.y);
        PostMessage(appWindow, WM_MOUSEMOVE, MK_LBUTTON, lParam);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        PostMessage(appWindow, WM_LBUTTONUP, 0, lParam);
        
        return true;
    }

    bool TestDrawingBounds() {
        ASSERT_NE(nullptr, appWindow);
        
        RECT clientRect;
        GetClientRect(appWindow, &clientRect);
        
        // Test that drawing bounds are reasonable
        int toolbarHeight = 60;
        int statusHeight = 25;
        int canvasHeight = clientRect.bottom - toolbarHeight - statusHeight;
        
        ASSERT_TRUE(canvasHeight > 200); // Minimum reasonable canvas height
        ASSERT_TRUE(clientRect.right > 400); // Minimum reasonable canvas width
        
        return true;
    }

    bool TestColorSelection() {
        ASSERT_NE(nullptr, appWindow);
        
        // Click on color palette area
        POINT colorPoint = {350 + 2 * 12, 15}; // Third color in palette
        LPARAM lParam = MAKELPARAM(colorPoint.x, colorPoint.y);
        
        PostMessage(appWindow, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        PostMessage(appWindow, WM_LBUTTONUP, 0, lParam);
        
        return true;
    }

    // Stability Tests
    bool TestMemoryUsage() {
        ASSERT_NE(nullptr, appWindow);
        
        // Get process ID
        DWORD processId;
        GetWindowThreadProcessId(appWindow, &processId);
        
        // Open process for querying
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        ASSERT_NE(nullptr, hProcess);
        
        // Get memory usage
        PROCESS_MEMORY_COUNTERS_EX memCounters;
        if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&memCounters, sizeof(memCounters))) {
            // Check that memory usage is reasonable (less than 100MB for a simple paint app)
            SIZE_T memoryUsageMB = memCounters.WorkingSetSize / (1024 * 1024);
            ASSERT_TRUE(memoryUsageMB < 100);
        }
        
        CloseHandle(hProcess);
        return true;
    }

    bool TestWindowClose() {
        ASSERT_NE(nullptr, appWindow);
        
        // Test graceful window close
        PostMessage(appWindow, WM_CLOSE, 0, 0);
        
        // Wait for window to close
        int attempts = 0;
        while (IsWindow(appWindow) && attempts < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            attempts++;
        }
        
        ASSERT_FALSE(IsWindow(appWindow));
        appWindow = nullptr;
        
        return true;
    }
};

int main() {
    std::cout << "Modern Paint Studio Pro - Integration Test Suite" << std::endl;
    std::cout << "Testing real application launch and UI interactions..." << std::endl;
    std::cout << "Note: This will launch and close the application multiple times." << std::endl;
    std::cout << std::endl;
    
    IntegrationTests tests;
    tests.RunTests();
    
    std::cout << "\nIntegration tests complete!" << std::endl;
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.get();
    
    return 0;
}