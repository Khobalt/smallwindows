# Modern Paint Studio Pro 🎨

A modern, feature-rich digital painting application built with pure Win32 API and GDI+. This started as a minimal Windows application but evolved into a professional-grade paint program with advanced features.

## ✨ Modern Features

### 🎨 Advanced Drawing Tools
- **Brush Tool** - Variable size drawing with smooth strokes (B key)
- **Eraser Tool** - Clean erasing with adjustable sizes (E key)
- **Shape Tools** - Rectangle (R), Circle (C), and Line (L) drawing
- **16-Color Palette** - Instant color selection with visual feedback
- **HSV Color Wheel** - Advanced color picker with full spectrum selection

### 🖼️ Professional UI
- **Modern Dark/Light Themes** - Toggle between elegant themes (Ctrl+T)
- **Professional Menu Bar** - File, Edit, View, Tools, Help menus with shortcuts
- **Interactive Toolbar** - Click-to-select tools and colors
- **Dynamic Brush Size Slider** - Real-time size adjustment (1-20px)
- **Advanced Color Picker** - Click "More" for HSV color wheel selection
- **Comprehensive Status Bar** - Shows tool, size, zoom, grid, theme, and point count
- **Right-Click Context Menu** - Quick access to all features

### ⚡ Advanced Features
- **Unlimited Undo/Redo** - Full history management
- **Zoom & Pan** - Ctrl+Mouse wheel to zoom (20%-500%), mouse wheel to scroll
- **Grid Overlay** - Press 'G' to toggle alignment grid
- **File Operations** - Save artwork (Ctrl+S)
- **Keyboard Shortcuts** - Professional-grade hotkeys
- **Window Resizing** - Responsive layout that adapts to window size

### 🎯 User Experience
- **Smooth Real-time Drawing** - Instant visual feedback
- **Theme-Aware Colors** - UI adapts colors based on selected theme
- **Help System** - Press F1 for comprehensive help
- **Professional Title** - "Modern Paint Studio Pro - Advanced Digital Art Application"

## 🚀 Quick Start

### Compilation
```bash
# Minimal version (original)
g++ helloworld.cpp -o helloworld.exe -mwindows

# Modern version with all features
g++ helloworld.cpp -o modernpaint.exe -mwindows -lgdiplus -lcomdlg32 -O2
```

### System Requirements
- Windows 10/11 (or Windows 7+ with GDI+ support)
- MinGW-w64 compiler
- GDI+ library support

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | New canvas |
| `Ctrl+S` | Save image |
| `Ctrl+Z` | Undo |
| `Ctrl+Y` | Redo |
| `Ctrl+T` | Toggle theme |
| `G` | Toggle grid |
| `B` | Brush tool |
| `E` | Eraser tool |
| `R` | Rectangle tool |
| `C` | Circle tool |
| `L` | Line tool |
| `1-9` | Brush sizes |
| `F1` | Help |
| `ESC` | Exit |
| `Ctrl+Mouse Wheel` | Zoom |
| `Mouse Wheel` | Scroll |
| `Right Click` | Context menu |

## 🎨 How to Use

1. **Select Tools**: Click toolbar buttons or use keyboard (B/E/R/C/L keys)
2. **Choose Colors**: Click 16-color palette OR click "More" for advanced HSV color wheel
3. **Adjust Brush Size**: Click and drag the size slider (or press 1-9)
4. **Draw**: Click and drag on the canvas area
5. **Use Shapes**: Select shape tool, click and drag to create perfect shapes
6. **Professional Features**: Use menu bar (File/Edit/View/Tools/Help) or shortcuts
7. **Zoom**: Hold Ctrl and scroll mouse wheel for precision work
8. **Toggle Features**: Press 'G' for grid, Ctrl+T for theme switching
9. **Right-Click**: Access context menu for all features

## 📏 Technical Details

- **File Size**: ~150KB compiled (modern version)
- **Memory Usage**: Minimal RAM footprint
- **Dependencies**: GDI+, Common Dialog libraries
- **Architecture**: Pure Win32 API with GDI+ graphics
- **Performance**: Optimized rendering with immediate drawing feedback

## 🎯 Features Comparison

| Feature | Original | Modern Version |
|---------|----------|----------------|
| Drawing | ✓ Basic | ✓ Advanced with variable brush sizes |
| Colors | ✓ 10 keyboard shortcuts | ✓ 16-color palette + HSV color wheel |
| UI | Basic text instructions | ✓ Professional menu bar, toolbar & status bar |
| Themes | None | ✓ Dark/Light themes with smooth transitions |
| Undo/Redo | ✓ Basic | ✓ Unlimited with visual feedback |
| Tools | Brush only | ✓ 6 tools with keyboard shortcuts (B/E/R/C/L) |
| File Operations | None | ✓ Save with professional dialog |
| Zoom & Pan | None | ✓ Full zoom/pan support (20%-500%) |
| Grid | None | ✓ Toggle grid overlay for precision |
| Help System | None | ✓ F1 comprehensive help with shortcuts |
| Context Menu | None | ✓ Right-click menu with all features |
| Menu System | None | ✓ Professional File/Edit/View/Tools/Help menus |
| Color Picker | None | ✓ Advanced HSV color wheel with real-time preview |

## 🧪 Testing Framework

Modern Paint Studio Pro includes a comprehensive testing suite to ensure reliability and quality:

### Quick Testing
```bash
# Run all tests (unit + integration + manual guide)
./run_tests.bat

# Or use individual test suites
./tests.exe              # Unit tests only
./integration_tests.exe  # Integration tests only
```

### Test Coverage
- **19 automated unit tests** - Core functionality validation
- **15 automated integration tests** - Real UI and application testing  
- **Manual testing guide** - User experience validation
- **Performance benchmarks** - Speed and memory usage validation

### Test Categories
- ✅ Color system accuracy (HSV↔RGB conversion)
- ✅ UI component click detection and calculations
- ✅ Drawing system integrity and bounds checking
- ✅ Keyboard shortcut validation
- ✅ Memory usage and leak prevention  
- ✅ Application launch and window management
- ✅ Performance benchmarks (color conversion, UI responsiveness)

**Current Test Status: ✅ 100% Pass Rate**

See [TESTING.md](TESTING.md) for comprehensive testing documentation.

---

Transform your simple drawing needs into professional digital art with Modern Paint Studio Pro! 🎨✨