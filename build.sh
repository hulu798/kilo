#!/bin/bash

# Build script for Kilo text editor
# This script builds both the modular and original versions

echo "Building Kilo text editor..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building..."
make

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Executables created:"
    echo "run:"
    echo "  ./build/kilo <filename>"
else
    echo "Build failed!"
    exit 1
fi
