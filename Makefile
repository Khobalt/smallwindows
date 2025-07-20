# Modern Paint Studio Pro - Makefile
# Build system for main application and test suite

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lgdiplus -lcomdlg32
WINFLAGS = -mwindows

# Source files
MAIN_SRC = helloworld.cpp
TEST_SRC = tests.cpp test_stubs.cpp
COMPREHENSIVE_TEST_SRC = comprehensive_tests.cpp test_stubs.cpp
MESSAGE_TEST_SRC = message_handler_tests.cpp
DRAWING_TEST_SRC = drawing_function_tests.cpp test_stubs.cpp
TEST_FRAMEWORK = test_framework.h

# Output executables
MAIN_EXE = modernpaint.exe
TEST_EXE = tests.exe
COMPREHENSIVE_EXE = comprehensive_tests.exe
MESSAGE_EXE = message_handler_tests.exe
DRAWING_EXE = drawing_function_tests.exe
INTEGRATION_EXE = integration_tests.exe

# Default target - build main application
all: $(MAIN_EXE)

# Build main application
$(MAIN_EXE): $(MAIN_SRC)
	@echo "Building Modern Paint Studio Pro..."
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN_EXE) $(WINFLAGS) $(LIBS)
	@echo "✓ Build complete: $(MAIN_EXE)"

# Build test suite
tests: $(TEST_EXE)

$(TEST_EXE): $(TEST_SRC) $(TEST_FRAMEWORK)
	@echo "Building test suite..."
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_EXE) $(LIBS)
	@echo "✓ Test build complete: $(TEST_EXE)"

# Build comprehensive tests
comprehensive-tests: $(COMPREHENSIVE_EXE)

$(COMPREHENSIVE_EXE): $(COMPREHENSIVE_TEST_SRC) $(TEST_FRAMEWORK)
	@echo "Building comprehensive test suite..."
	$(CXX) $(CXXFLAGS) $(COMPREHENSIVE_TEST_SRC) -o $(COMPREHENSIVE_EXE) $(LIBS)
	@echo "✓ Comprehensive test build complete: $(COMPREHENSIVE_EXE)"

# Build message handler tests
message-tests: $(MESSAGE_EXE)

$(MESSAGE_EXE): $(MESSAGE_TEST_SRC) $(TEST_FRAMEWORK)
	@echo "Building message handler test suite..."
	$(CXX) $(CXXFLAGS) $(MESSAGE_TEST_SRC) -o $(MESSAGE_EXE) $(LIBS)
	@echo "✓ Message handler test build complete: $(MESSAGE_EXE)"

# Build drawing function tests
drawing-tests: $(DRAWING_EXE)

$(DRAWING_EXE): $(DRAWING_TEST_SRC) $(TEST_FRAMEWORK)
	@echo "Building drawing function test suite..."
	$(CXX) $(CXXFLAGS) $(DRAWING_TEST_SRC) -o $(DRAWING_EXE) $(LIBS)
	@echo "✓ Drawing function test build complete: $(DRAWING_EXE)"

# Build integration tests
integration-tests: $(INTEGRATION_EXE)

$(INTEGRATION_EXE): integration_tests.cpp $(TEST_FRAMEWORK)
	@echo "Building integration test suite..."
	$(CXX) $(CXXFLAGS) integration_tests.cpp -o $(INTEGRATION_EXE) $(LIBS) -lpsapi
	@echo "✓ Integration test build complete: $(INTEGRATION_EXE)"

# Run unit tests
test: $(TEST_EXE)
	@echo "Running unit test suite..."
	@echo "======================================"
	./$(TEST_EXE)

# Run integration tests
test-integration: $(INTEGRATION_EXE)
	@echo "Running integration test suite..."
	@echo "======================================"
	./$(INTEGRATION_EXE)

# Run comprehensive function tests
test-comprehensive: $(COMPREHENSIVE_EXE)
	@echo "Running comprehensive function tests..."
	@echo "======================================"
	./$(COMPREHENSIVE_EXE)

# Run message handler tests
test-messages: $(MESSAGE_EXE)
	@echo "Running message handler tests..."
	@echo "======================================"
	./$(MESSAGE_EXE)

# Run drawing function tests
test-drawing: $(DRAWING_EXE)
	@echo "Running drawing function tests..."
	@echo "======================================"
	./$(DRAWING_EXE)

# Run all tests (comprehensive)
test-all: $(TEST_EXE) $(COMPREHENSIVE_EXE) $(MESSAGE_EXE) $(DRAWING_EXE) $(INTEGRATION_EXE)
	@echo "Running ALL test suites..."
	@echo "======================================"
	./run_tests.bat

# Run main application
run: $(MAIN_EXE)
	@echo "Launching Modern Paint Studio Pro..."
	./$(MAIN_EXE)

# Build everything
build-all: $(MAIN_EXE) $(TEST_EXE) $(COMPREHENSIVE_EXE) $(MESSAGE_EXE) $(DRAWING_EXE) $(INTEGRATION_EXE)
	@echo "✓ All builds complete!"

# Test and run sequence
test-and-run: test run

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	-rm -f $(MAIN_EXE) $(TEST_EXE) $(INTEGRATION_EXE) *.o
	@echo "✓ Clean complete"

# Development build (with debug info)
debug: CXXFLAGS += -g -DDEBUG
debug: $(MAIN_EXE)
	@echo "✓ Debug build complete"

# Release build (optimized)
release: CXXFLAGS += -O3 -DNDEBUG
release: $(MAIN_EXE)
	@echo "✓ Release build complete"

# Continuous integration build
ci: clean build-all test
	@echo "✓ CI build and test complete"

# Help target
help:
	@echo "Modern Paint Studio Pro - Build System"
	@echo "======================================"
	@echo "Available targets:"
	@echo "  all         - Build main application (default)"
	@echo "  tests       - Build test suite"
	@echo "  test        - Build and run test suite"
	@echo "  run         - Build and run main application"
	@echo "  build-all   - Build both main app and tests"
	@echo "  test-and-run- Run tests then launch app"
	@echo "  clean       - Remove build artifacts"
	@echo "  debug       - Build with debug symbols"
	@echo "  release     - Build optimized release version"
	@echo "  ci          - Full CI build and test"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build main application"
	@echo "  make test         # Run test suite"
	@echo "  make test-and-run # Test then launch app"
	@echo "  make clean all    # Clean build and rebuild"

# Phony targets
.PHONY: all tests test run build-all test-and-run clean debug release ci help

# Default goal
.DEFAULT_GOAL := all