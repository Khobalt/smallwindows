# Modern Paint Studio Pro - Refactored Build System
# Professional C++ build system with modular architecture

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lgdiplus -lcomdlg32 -ld2d1 -ldwrite -lwindowscodecs -lole32
WINFLAGS = -mwindows

# Directory structure
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

# Source files organized by module
CORE_SOURCES = $(SRC_DIR)/core/types.cpp $(SRC_DIR)/core/config.cpp $(SRC_DIR)/core/app_state.cpp $(SRC_DIR)/core/event_handler.cpp
UI_SOURCES = $(SRC_DIR)/ui/ui_renderer.cpp $(SRC_DIR)/ui/gpu_ui_renderer.cpp $(SRC_DIR)/ui/icon_renderer.cpp
DRAWING_SOURCES = $(SRC_DIR)/drawing/drawing_engine.cpp
RENDERING_SOURCES = $(SRC_DIR)/rendering/gpu_renderer.cpp
MAIN_SOURCE = $(SRC_DIR)/main.cpp

# All application sources
APP_SOURCES = $(CORE_SOURCES) $(UI_SOURCES) $(DRAWING_SOURCES) $(RENDERING_SOURCES) $(MAIN_SOURCE)

# Test sources
TEST_FRAMEWORK = $(TEST_DIR)/test_framework.h
TEST_STUBS = $(TEST_DIR)/test_stubs.cpp
UNIT_TESTS = $(shell find $(TEST_DIR)/unit -name "*.cpp" 2>/dev/null || echo "")

# Object files
CORE_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CORE_SOURCES))
UI_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(UI_SOURCES))
DRAWING_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(DRAWING_SOURCES))
RENDERING_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(RENDERING_SOURCES))
MAIN_OBJECT = $(BUILD_DIR)/main.o
APP_OBJECTS = $(CORE_OBJECTS) $(UI_OBJECTS) $(DRAWING_OBJECTS) $(RENDERING_OBJECTS) $(MAIN_OBJECT)

# Executables
MAIN_EXE = $(BIN_DIR)/modernpaint.exe
TEST_EXE = $(BIN_DIR)/tests.exe

# Include path
INCLUDES = -I$(INC_DIR)

# Default target - build main application
all: $(MAIN_EXE)

# Create directories if they don't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/core $(BUILD_DIR)/ui $(BUILD_DIR)/drawing $(BUILD_DIR)/rendering
	@echo "✓ Created build directories"

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)
	@echo "✓ Created bin directory"

# Build main application
$(MAIN_EXE): $(BUILD_DIR) $(BIN_DIR) $(APP_OBJECTS)
	@echo "🔗 Linking Modern Paint Studio Pro..."
	$(CXX) $(CXXFLAGS) $(APP_OBJECTS) -o $(MAIN_EXE) $(WINFLAGS) $(LIBS)
	@echo "✅ Build complete: $(MAIN_EXE)"

# Object file rules
$(BUILD_DIR)/core/%.o: $(SRC_DIR)/core/%.cpp
	@echo "🔨 Compiling core module: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/ui/%.o: $(SRC_DIR)/ui/%.cpp
	@echo "🎨 Compiling UI module: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/drawing/%.o: $(SRC_DIR)/drawing/%.cpp
	@echo "✏️ Compiling drawing module: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/rendering/%.o: $(SRC_DIR)/rendering/%.cpp
	@echo "🎮 Compiling GPU rendering module: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	@echo "🚀 Compiling main application: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Test targets (if test files exist)
test: $(TEST_EXE)
	@echo "🧪 Running test suite..."
	@echo "======================================"
	@if [ -f "$(TEST_EXE)" ]; then ./$(TEST_EXE); else echo "⚠️  No tests available"; fi

$(TEST_EXE): $(BIN_DIR)
	@if [ -n "$(UNIT_TESTS)" ]; then \
		echo "🔨 Building test suite..."; \
		$(CXX) $(CXXFLAGS) $(INCLUDES) $(UNIT_TESTS) $(TEST_STUBS) -o $(TEST_EXE) $(LIBS); \
		echo "✅ Test build complete: $(TEST_EXE)"; \
	else \
		echo "⚠️  No unit test files found"; \
		touch $(TEST_EXE); \
	fi

# Run main application
run: $(MAIN_EXE)
	@echo "🚀 Launching Modern Paint Studio Pro..."
	./$(MAIN_EXE)

# Development build with debug info
debug: CXXFLAGS += -g -DDEBUG
debug: $(MAIN_EXE)
	@echo "🐛 Debug build complete"

# Release build (optimized)
release: CXXFLAGS += -O3 -DNDEBUG
release: clean $(MAIN_EXE)
	@echo "🚀 Release build complete"

# Test and run sequence
test-and-run: test run

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)/*.exe $(BIN_DIR)/*.o
	@echo "✅ Clean complete"

# Clean everything including directories
clean-all: clean
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✅ Full clean complete"

# Show project structure
structure:
	@echo "📁 Modern Paint Studio Pro - Project Structure"
	@echo "==============================================="
	@echo "📦 Root"
	@echo "├── 🔧 Makefile (this build system)"
	@echo "├── 📁 src/ (source code)"
	@echo "│   ├── 📁 core/ (application core)"
	@echo "│   ├── 📁 ui/ (user interface)"
	@echo "│   ├── 📁 drawing/ (drawing engine)"
	@echo "│   └── 🚀 main.cpp (entry point)"
	@echo "├── 📁 include/ (header files)"
	@echo "├── 📁 tests/ (test suite)"
	@echo "├── 📁 docs/ (documentation)"
	@echo "├── 📁 build/ (build artifacts)"
	@echo "└── 📁 bin/ (executables)"

# Show available targets
help:
	@echo "Modern Paint Studio Pro - Build System"
	@echo "======================================"
	@echo "Available targets:"
	@echo "  📦 all         - Build main application (default)"
	@echo "  🧪 test        - Build and run test suite"
	@echo "  🚀 run         - Build and run main application"
	@echo "  🐛 debug       - Build with debug symbols"
	@echo "  🚀 release     - Build optimized release version"
	@echo "  🎯 test-and-run- Run tests then launch app"
	@echo "  🧹 clean       - Remove build artifacts"
	@echo "  🗑️  clean-all   - Remove all build files and directories"
	@echo "  📁 structure   - Show project structure"
	@echo "  ❓ help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build main application"
	@echo "  make test         # Run test suite"
	@echo "  make run          # Build and launch app"
	@echo "  make release      # Create optimized build"
	@echo "  make clean all    # Clean and rebuild"

# Phony targets
.PHONY: all test run debug release test-and-run clean clean-all structure help

# Default goal
.DEFAULT_GOAL := all