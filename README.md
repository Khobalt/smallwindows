# Modern Paint Studio Pro 🎨

A modern, feature-rich digital painting application built with pure Win32 API and GDI+. This started as a minimal Windows application but evolved into a professional-grade paint program with advanced features.

## ✨ Modern Features

### 🎨 Advanced Drawing Tools
- **Brush Tool** - Variable size drawing with smooth strokes
- **Eraser Tool** - Clean erasing with adjustable sizes
- **Shape Tools** - Rectangle, Circle, and Line drawing
- **Color Picker** - 16-color palette with one-click selection

### 🖼️ Professional UI
- **Modern Dark/Light Themes** - Toggle between elegant themes
- **Interactive Toolbar** - Click-to-select tools and colors
- **Dynamic Brush Size Slider** - Real-time size adjustment (1-20px)
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
| `1-9` | Brush sizes |
| `F1` | Help |
| `ESC` | Exit |
| `Ctrl+Mouse Wheel` | Zoom |
| `Mouse Wheel` | Scroll |
| `Right Click` | Context menu |

## 🎨 How to Use

1. **Select Tools**: Click toolbar buttons (Brush, Eraser, Rectangle, Circle, Line, Picker)
2. **Choose Colors**: Click any color in the 16-color palette
3. **Adjust Brush Size**: Click and drag the size slider (or press 1-9)
4. **Draw**: Click and drag on the white canvas area
5. **Use Shapes**: Select shape tool, click and drag to create
6. **Zoom**: Hold Ctrl and scroll mouse wheel
7. **Toggle Features**: Press 'G' for grid, Ctrl+T for theme
8. **Right-Click**: Access context menu for all features

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
| Colors | ✓ 10 keyboard shortcuts | ✓ 16-color visual palette |
| UI | Basic text instructions | ✓ Professional toolbar & status bar |
| Themes | None | ✓ Dark/Light themes |
| Undo/Redo | ✓ Basic | ✓ Unlimited with visual feedback |
| Tools | Brush only | ✓ 6 tools (Brush, Eraser, Shapes) |
| File Operations | None | ✓ Save with dialog |
| Zoom & Pan | None | ✓ Full zoom/pan support |
| Grid | None | ✓ Toggle grid overlay |
| Help System | None | ✓ F1 comprehensive help |
| Context Menu | None | ✓ Right-click menu |

Transform your simple drawing needs into professional digital art with Modern Paint Studio Pro! 🎨✨