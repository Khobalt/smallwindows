# Modern Paint Studio Pro - Refactored Architecture

A professional digital art application built with C++ and Win32, featuring a modern modular architecture.

## 🚀 Quick Start

### Build & Run
```bash
# Compile all modules
g++ -std=c++17 -Wall -O2 -Iinclude \
    src/core/*.cpp src/ui/*.cpp src/drawing/*.cpp src/main.cpp \
    -o bin/modernpaint.exe -mwindows -lgdiplus -lcomdlg32

# Run the application
./bin/modernpaint.exe
```

## 📁 Project Structure

```
📦 Modern Paint Studio Pro
├── 🔧 Makefile                 # Professional build system
├── 📁 src/                     # Source code (modular)
│   ├── 📁 core/                # Application core
│   ├── 📁 ui/                  # User interface
│   ├── 📁 drawing/             # Drawing engine
│   └── main.cpp                # Entry point
├── 📁 include/                 # Header files
├── 📁 tests/                   # Test suite
├── 📁 docs/                    # Documentation
├── 📁 build/                   # Build artifacts
└── 📁 bin/                     # Executables
```

## ✨ Features

### 🎨 **Drawing Tools**
- **Brush Tool** - Freehand drawing with adjustable size
- **Eraser Tool** - Remove content with precision
- **Shape Tools** - Rectangle, circle, and line tools
- **Color Picker** - Advanced HSV color selection

### 🎯 **Professional UI**
- **Modern Interface** - Clean, responsive design
- **Theme System** - Light/dark theme support
- **Toolbar** - Easy access to all tools and settings
- **Status Bar** - Real-time information display
- **Menu System** - Complete menu with shortcuts

### ⚡ **Advanced Features**
- **Undo/Redo System** - Full operation history
- **Zoom & Pan** - Navigate large canvases
- **Grid Overlay** - Precision drawing assistance
- **File Operations** - Save/load artwork
- **Keyboard Shortcuts** - Professional workflow

## 🏗️ Architecture

### **Modular Design**
- **Core Module**: State management, configuration, events
- **UI Module**: Rendering, menus, interface elements  
- **Drawing Module**: Graphics operations, tools, algorithms
- **Clean Interfaces**: Well-defined APIs between modules

### **Professional Patterns**
- **Singleton State**: Centralized application state
- **Namespace Organization**: Clear module boundaries
- **Event-Driven**: Proper message handling architecture
- **Separation of Concerns**: Logic separated from presentation

## 🔧 Development

### **Build System**
The project uses a professional modular build system:

```bash
# If make is available:
make all         # Build application
make debug       # Debug build  
make release     # Optimized build
make structure   # Show project layout
make help        # Available commands

# Direct compilation:
# Compile each module to object files, then link
```

### **Testing**
Comprehensive test suite with professional framework:
- Unit tests for all major components
- Integration tests for full application
- Performance and stress testing
- Automated test runner

## 📚 Documentation

- **[REFACTORING.md](docs/REFACTORING.md)** - Detailed refactoring guide
- **[TESTING.md](docs/TESTING.md)** - Test suite documentation  
- **Header Files** - Comprehensive API documentation

## 🎯 Key Improvements

### **Before Refactoring**
- ❌ Single monolithic 1000+ line file
- ❌ Mixed concerns and responsibilities  
- ❌ Difficult to maintain and extend
- ❌ Hard to test individual components

### **After Refactoring** 
- ✅ **Modular Architecture** - Clean separation of concerns
- ✅ **Professional Structure** - Industry-standard organization
- ✅ **Maintainable Code** - Easy to understand and modify
- ✅ **Testable Components** - Individual module testing
- ✅ **Scalable Design** - Easy to extend with new features
- ✅ **Team-Ready** - Multiple developers can work simultaneously

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| **Ctrl+Z** | Undo last action |
| **Ctrl+Y** | Redo action |
| **Ctrl+S** | Save image |
| **Ctrl+N** | New canvas |
| **Ctrl+T** | Toggle theme |
| **G** | Toggle grid |
| **B/E/R/C/L** | Select tools |
| **1-9** | Brush sizes |
| **F1** | Help |

## 🛠️ Requirements

- **Windows OS** with Win32 API support
- **GDI+** graphics library  
- **C++17** compatible compiler (MinGW, MSVC)
- **Standard Libraries**: `<windows.h>`, `<gdiplus.h>`, `<vector>`, etc.

## 🎨 Modern Paint Studio Pro

**A professional digital art application showcasing excellent C++ architecture, clean code practices, and modular design principles.**

Transform your digital art workflow with this fully-featured painting application!