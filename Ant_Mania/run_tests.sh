#!/bin/bash

echo "Building and running Ant Mania tests with Google Test..."

# Check if Google Test is available
if ! pkg-config --exists gtest; then
    echo "Google Test not found. Installing via Homebrew..."
    if command -v brew &> /dev/null; then
        brew install googletest
    else
        echo "Homebrew not found. Please install Google Test manually."
        echo "On macOS: brew install googletest"
        echo "On Ubuntu: sudo apt-get install libgtest-dev"
        exit 1
    fi
fi

# Create test build directory
mkdir -p test/build
cd test/build

# Configure and build tests
echo "Configuring tests with CMake..."
cmake -DCMAKE_BUILD_TYPE=Debug ..

echo "Building tests..."
make test_ant_mania

if [ $? -eq 0 ]; then
    echo ""
    echo "Running tests..."
    echo "=================="
    ./test_ant_mania
    echo "=================="
    echo ""
    echo "Running tests via CTest..."
    ctest --verbose
    echo ""
    echo "Tests completed!"
else
    echo "Build failed!"
    exit 1
fi