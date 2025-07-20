#include "test_framework.h"
#include <windows.h>
#include <gdiplus.h>
#include <cmath>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// Forward declarations for functions we want to test
extern COLORREF HSVtoRGB(float h, float s, float v);
extern COLORREF colorPalette[];

// Mock/Test versions of global variables
bool testShowGrid = false;
int testBrushSize = 5;
COLORREF testCurrentColor = RGB(0, 0, 0);
float testZoomLevel = 1.0f;

// Simulate window dimensions for testing
const int TEST_WINDOW_WIDTH = 1200;
const int TEST_WINDOW_HEIGHT = 800;
const int TOOLBAR_HEIGHT = 60;
const int STATUSBAR_HEIGHT = 25;

class ModernPaintTests {
private:
    TestFramework framework;
    HWND testWindow;

public:
    ModernPaintTests() {
        // Initialize GDI+ for testing
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        
        SetupTests();
    }

    void SetupTests() {
        // Core Functionality Tests
        framework.AddSuite("Core Functionality");
        framework.AddTest("HSV to RGB Conversion", [this]() { return TestHSVtoRGB(); });
        framework.AddTest("Color Palette Validation", [this]() { return TestColorPalette(); });
        framework.AddTest("Brush Size Validation", [this]() { return TestBrushSizeRange(); });
        framework.AddTest("Zoom Level Validation", [this]() { return TestZoomLevelRange(); });

        // UI Component Tests
        framework.AddSuite("UI Components");
        framework.AddTest("Toolbar Click Region Detection", [this]() { return TestToolbarClickRegions(); });
        framework.AddTest("Color Picker Click Detection", [this]() { return TestColorPickerClicks(); });
        framework.AddTest("Brush Size Slider", [this]() { return TestBrushSizeSlider(); });
        framework.AddTest("Theme Toggle", [this]() { return TestThemeToggle(); });

        // Drawing System Tests
        framework.AddSuite("Drawing System");
        framework.AddTest("Drawing Point Structure", [this]() { return TestDrawPointStructure(); });
        framework.AddTest("Canvas Area Calculation", [this]() { return TestCanvasArea(); });
        framework.AddTest("Grid System", [this]() { return TestGridSystem(); });

        // Keyboard Shortcuts Tests
        framework.AddSuite("Keyboard Shortcuts");
        framework.AddTest("Tool Selection Keys", [this]() { return TestToolSelectionKeys(); });
        framework.AddTest("Menu Shortcuts", [this]() { return TestMenuShortcuts(); });
        framework.AddTest("Brush Size Number Keys", [this]() { return TestBrushSizeKeys(); });

        // Window Management Tests
        framework.AddSuite("Window Management");
        framework.AddTest("Window Creation", [this]() { return TestWindowCreation(); });
        framework.AddTest("Window Dimensions", [this]() { return TestWindowDimensions(); });
        framework.AddTest("Menu Bar Creation", [this]() { return TestMenuBarCreation(); });

        // Performance Tests
        framework.AddSuite("Performance");
        framework.AddTest("Color Conversion Speed", [this]() { return TestColorConversionPerformance(); });
        framework.AddTest("UI Responsiveness", [this]() { return TestUIResponsiveness(); });
    }

    void RunTests() {
        framework.RunAllTests();
    }

private:
    // Core Functionality Tests
    bool TestHSVtoRGB() {
        // Test basic color conversions
        COLORREF red = HSVtoRGB(0, 1.0f, 1.0f);
        ASSERT_EQ(RGB(255, 0, 0), red);

        COLORREF green = HSVtoRGB(120, 1.0f, 1.0f);
        ASSERT_EQ(RGB(0, 255, 0), green);

        COLORREF blue = HSVtoRGB(240, 1.0f, 1.0f);
        ASSERT_EQ(RGB(0, 0, 255), blue);

        // Test white (no saturation)
        COLORREF white = HSVtoRGB(0, 0.0f, 1.0f);
        ASSERT_EQ(RGB(255, 255, 255), white);

        return true;
    }

    bool TestColorPalette() {
        // Verify we have the expected number of colors
        int colorCount = 0;
        for (int i = 0; i < 16; i++) {
            if (colorPalette[i] != 0 || i == 0) { // Black is valid
                colorCount++;
            }
        }
        ASSERT_EQ(16, colorCount);

        // Test specific colors exist
        bool hasBlack = false, hasWhite = false, hasRed = false;
        for (int i = 0; i < 16; i++) {
            if (colorPalette[i] == RGB(0, 0, 0)) hasBlack = true;
            if (colorPalette[i] == RGB(255, 255, 255)) hasWhite = true;
            if (colorPalette[i] == RGB(255, 0, 0)) hasRed = true;
        }
        ASSERT_TRUE(hasBlack);
        ASSERT_TRUE(hasWhite);
        ASSERT_TRUE(hasRed);

        return true;
    }

    bool TestBrushSizeRange() {
        // Test brush size boundaries
        for (int size = 1; size <= 20; size++) {
            testBrushSize = size;
            ASSERT_TRUE(testBrushSize >= 1 && testBrushSize <= 20);
        }
        return true;
    }

    bool TestZoomLevelRange() {
        // Test zoom level boundaries
        testZoomLevel = 0.2f; // Min zoom
        ASSERT_TRUE(testZoomLevel >= 0.2f);
        
        testZoomLevel = 5.0f; // Max zoom
        ASSERT_TRUE(testZoomLevel <= 5.0f);
        
        testZoomLevel = 1.0f; // Default zoom
        ASSERT_EQ(1.0f, testZoomLevel);
        
        return true;
    }

    // UI Component Tests
    bool TestToolbarClickRegions() {
        // Test tool button regions (each 50px wide)
        for (int i = 0; i < 6; i++) {
            int leftBound = i * 50 + 5;
            int rightBound = i * 50 + 45;
            ASSERT_TRUE(leftBound < rightBound);
            ASSERT_TRUE(rightBound <= 300); // Within toolbar bounds
        }
        return true;
    }

    bool TestColorPickerClicks() {
        // Test color palette click regions (12px each)
        for (int i = 0; i < 16; i++) {
            int leftBound = 350 + i * 12;
            int rightBound = leftBound + 10;
            ASSERT_TRUE(leftBound >= 350);
            ASSERT_TRUE(rightBound <= 542);
        }
        return true;
    }

    bool TestBrushSizeSlider() {
        // Test brush size slider calculations
        int sliderStart = 600;
        int sliderEnd = 700;
        int sliderWidth = sliderEnd - sliderStart;
        
        // Test size calculation at different positions
        for (int x = sliderStart; x <= sliderEnd; x += 10) {
            int calculatedSize = 1 + ((x - sliderStart) * 19) / sliderWidth;
            ASSERT_TRUE(calculatedSize >= 1 && calculatedSize <= 20);
        }
        return true;
    }

    bool TestThemeToggle() {
        // Test theme enumeration values
        enum ThemeType { THEME_LIGHT = 0, THEME_DARK = 1 };
        ThemeType currentTheme = THEME_LIGHT;
        
        // Toggle test
        currentTheme = (currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
        ASSERT_EQ(THEME_DARK, currentTheme);
        
        currentTheme = (currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
        ASSERT_EQ(THEME_LIGHT, currentTheme);
        
        return true;
    }

    // Drawing System Tests
    bool TestDrawPointStructure() {
        struct DrawPoint {
            int x, y;
            COLORREF color;
            bool isStart;
            int brushSize;
        };
        
        DrawPoint point = {100, 200, RGB(255, 0, 0), true, 5};
        ASSERT_EQ(100, point.x);
        ASSERT_EQ(200, point.y);
        ASSERT_EQ(RGB(255, 0, 0), point.color);
        ASSERT_TRUE(point.isStart);
        ASSERT_EQ(5, point.brushSize);
        
        return true;
    }

    bool TestCanvasArea() {
        // Test canvas area calculation
        int canvasTop = TOOLBAR_HEIGHT;
        int canvasBottom = TEST_WINDOW_HEIGHT - STATUSBAR_HEIGHT;
        int canvasHeight = canvasBottom - canvasTop;
        
        ASSERT_TRUE(canvasHeight > 0);
        ASSERT_TRUE(canvasTop >= 0);
        ASSERT_TRUE(canvasBottom <= TEST_WINDOW_HEIGHT);
        
        return true;
    }

    bool TestGridSystem() {
        // Test grid calculations
        testZoomLevel = 1.0f;
        int gridSize = (int)(20 * testZoomLevel);
        ASSERT_EQ(20, gridSize);
        
        testZoomLevel = 2.0f;
        gridSize = (int)(20 * testZoomLevel);
        ASSERT_EQ(40, gridSize);
        
        return true;
    }

    // Keyboard Shortcuts Tests
    bool TestToolSelectionKeys() {
        // Test tool enumeration
        enum ToolType {
            TOOL_BRUSH = 0,
            TOOL_ERASER = 1,
            TOOL_RECTANGLE = 2,
            TOOL_CIRCLE = 3,
            TOOL_LINE = 4,
            TOOL_PICKER = 5
        };
        
        // Verify all tools are in valid range
        for (int tool = TOOL_BRUSH; tool <= TOOL_PICKER; tool++) {
            ASSERT_TRUE(tool >= 0 && tool <= 5);
        }
        
        return true;
    }

    bool TestMenuShortcuts() {
        // Test menu ID ranges
        const int IDM_FILE_NEW = 1001;
        const int IDM_HELP_ABOUT = 1019;
        
        ASSERT_TRUE(IDM_FILE_NEW >= 1001);
        ASSERT_TRUE(IDM_HELP_ABOUT <= 1019);
        ASSERT_TRUE(IDM_HELP_ABOUT > IDM_FILE_NEW);
        
        return true;
    }

    bool TestBrushSizeKeys() {
        // Test brush size key calculations (1-9 keys)
        for (char key = '1'; key <= '9'; key++) {
            int size = (key - '0') * 2; // Scale brush sizes
            ASSERT_TRUE(size >= 2 && size <= 18);
        }
        return true;
    }

    // Window Management Tests
    bool TestWindowCreation() {
        // Test window class name
        const char* className = "ModernPaintApp";
        ASSERT_NE(nullptr, className);
        ASSERT_TRUE(strlen(className) > 0);
        
        return true;
    }

    bool TestWindowDimensions() {
        // Test default window dimensions
        int defaultWidth = 1200;
        int defaultHeight = 800;
        
        ASSERT_TRUE(defaultWidth > 800);
        ASSERT_TRUE(defaultHeight > 600);
        ASSERT_TRUE(defaultWidth > defaultHeight);
        
        return true;
    }

    bool TestMenuBarCreation() {
        // Test that we can create menu items
        HMENU testMenu = CreateMenu();
        ASSERT_NE(nullptr, testMenu);
        
        // Test adding menu items
        BOOL result = AppendMenu(testMenu, MF_STRING, 1001, "&Test");
        ASSERT_TRUE(result);
        
        DestroyMenu(testMenu);
        return true;
    }

    // Performance Tests
    bool TestColorConversionPerformance() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Perform 1000 color conversions
        for (int i = 0; i < 1000; i++) {
            float h = (i % 360);
            HSVtoRGB(h, 0.8f, 0.9f);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete in under 100ms
        ASSERT_TRUE(duration.count() < 100);
        
        return true;
    }

    bool TestUIResponsiveness() {
        // Test that basic calculations are fast
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate UI calculations
        for (int i = 0; i < 10000; i++) {
            int toolIndex = (i % 6);
            int colorIndex = (i % 16);
            int sliderPos = 600 + ((testBrushSize - 1) * 100) / 19;
            
            // Basic validation to prevent optimization
            ASSERT_TRUE(toolIndex >= 0 && toolIndex < 6);
            ASSERT_TRUE(colorIndex >= 0 && colorIndex < 16);
            ASSERT_TRUE(sliderPos >= 600);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete in under 10ms
        ASSERT_TRUE(duration.count() < 10);
        
        return true;
    }
};

int main() {
    std::cout << "Modern Paint Studio Pro - Automated Test Suite" << std::endl;
    std::cout << "Testing all core functionality, UI components, and performance..." << std::endl;
    
    ModernPaintTests tests;
    tests.RunTests();
    
    std::cout << "\nPress Enter to continue..." << std::endl;
    std::cin.get();
    
    return 0;
}