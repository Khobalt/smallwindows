#!/bin/bash

# Script to collect all source code files into a single text file
# Modern Paint Studio Pro - Source Code Collection

OUTPUT_FILE="all_source_code.txt"

echo "Modern Paint Studio Pro - Complete Source Code" > $OUTPUT_FILE
echo "Generated on: $(date)" >> $OUTPUT_FILE
echo "=============================================" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

# Function to add a file to the output with header
add_file() {
    local file=$1
    echo "=== FILE: $file ===" >> $OUTPUT_FILE
    echo "" >> $OUTPUT_FILE
    cat "$file" >> $OUTPUT_FILE
    echo "" >> $OUTPUT_FILE
    echo "" >> $OUTPUT_FILE
}

# Header files
echo "HEADER FILES:" >> $OUTPUT_FILE
echo "=============" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

for header in include/*.h; do
    if [ -f "$header" ]; then
        add_file "$header"
    fi
done

# Source files
echo "SOURCE FILES:" >> $OUTPUT_FILE
echo "=============" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

# Main source
if [ -f "src/main.cpp" ]; then
    add_file "src/main.cpp"
fi

# Core modules
echo "CORE MODULES:" >> $OUTPUT_FILE
echo "=============" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

for core_file in src/core/*.cpp; do
    if [ -f "$core_file" ]; then
        add_file "$core_file"
    fi
done

# Drawing modules
echo "DRAWING MODULES:" >> $OUTPUT_FILE
echo "================" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

for drawing_file in src/drawing/*.cpp; do
    if [ -f "$drawing_file" ]; then
        add_file "$drawing_file"
    fi
done

# Rendering modules
echo "RENDERING MODULES:" >> $OUTPUT_FILE
echo "==================" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

for rendering_file in src/rendering/*.cpp; do
    if [ -f "$rendering_file" ]; then
        add_file "$rendering_file"
    fi
done

# UI modules
echo "UI MODULES:" >> $OUTPUT_FILE
echo "===========" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

for ui_file in src/ui/*.cpp; do
    if [ -f "$ui_file" ]; then
        add_file "$ui_file"
    fi
done

# Build system
echo "BUILD SYSTEM:" >> $OUTPUT_FILE
echo "=============" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

if [ -f "Makefile" ]; then
    add_file "Makefile"
fi

# Summary
echo "SUMMARY:" >> $OUTPUT_FILE
echo "========" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE
echo "Total files collected:" >> $OUTPUT_FILE
find include src -name "*.h" -o -name "*.cpp" | wc -l >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE
echo "Total lines of code:" >> $OUTPUT_FILE
find include src -name "*.h" -o -name "*.cpp" -exec wc -l {} + | tail -1 >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE
echo "Collection complete!" >> $OUTPUT_FILE

echo "✅ Source code collection complete!"
echo "📄 Output file: $OUTPUT_FILE"
echo "📊 File size: $(du -h $OUTPUT_FILE | cut -f1)"