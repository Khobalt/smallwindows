@echo off
echo ====================================================
echo Modern Paint Studio Pro - Comprehensive Test Suite
echo ====================================================
echo.

:: Check if executables exist
if not exist "modernpaint.exe" (
    echo Building main application...
    make modernpaint.exe
    if errorlevel 1 (
        echo ERROR: Failed to build main application
        pause
        exit /b 1
    )
)

if not exist "tests.exe" (
    echo Building unit tests...
    make tests
    if errorlevel 1 (
        echo ERROR: Failed to build unit tests
        pause
        exit /b 1
    )
)

if not exist "integration_tests.exe" (
    echo Building integration tests...
    g++ -std=c++17 -Wall -O2 integration_tests.cpp -o integration_tests.exe -lgdiplus -lpsapi
    if errorlevel 1 (
        echo ERROR: Failed to build integration tests
        pause
        exit /b 1
    )
)

echo.
echo ====================================================
echo PHASE 1: Unit Tests
echo ====================================================
echo Running unit tests for core functionality...
echo.

tests.exe
if errorlevel 1 (
    echo.
    echo WARNING: Some unit tests failed
    echo Continuing with integration tests...
)

echo.
echo ====================================================
echo PHASE 2: Integration Tests
echo ====================================================
echo Running integration tests (will launch the app)...
echo.

integration_tests.exe
if errorlevel 1 (
    echo.
    echo WARNING: Some integration tests failed
)

echo.
echo ====================================================
echo PHASE 3: Manual Testing Guide
echo ====================================================
echo.
echo The following features should be tested manually:
echo.
echo 1. Launch modernpaint.exe
echo 2. Test all toolbar buttons (Brush, Eraser, Shapes)
echo 3. Try the color palette and "More" color picker
echo 4. Test keyboard shortcuts: B/E/R/C/L, Ctrl+Z/Y/S/N/T
echo 5. Try the menu bar: File, Edit, View, Tools, Help
echo 6. Test zoom (Ctrl+Mouse wheel) and grid (G key)
echo 7. Draw something and test undo/redo
echo 8. Try different brush sizes (1-9 keys or slider)
echo 9. Toggle between light and dark themes
echo 10. Right-click for context menu
echo.

choice /c YN /m "Would you like to launch the application for manual testing"
if errorlevel 2 goto :skip_manual
if errorlevel 1 goto :manual_test

:manual_test
echo.
echo Launching Modern Paint Studio Pro for manual testing...
start modernpaint.exe
echo.
echo The application is now running for your manual testing.
echo Close it when finished testing.
goto :end

:skip_manual
echo.
echo Skipping manual testing.

:end
echo.
echo ====================================================
echo Test Suite Complete!
echo ====================================================
echo.
echo Summary:
echo - Unit tests: Core functionality validation
echo - Integration tests: UI and application behavior
echo - Manual testing: User experience validation
echo.
echo Check the output above for any failed tests.
echo.
pause