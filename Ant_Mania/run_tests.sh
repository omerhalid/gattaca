#!/bin/bash

echo "Running tests..."

# Install gtest if needed
if ! pkg-config --exists gtest; then
    echo "Installing gtest..."
    if command -v brew &> /dev/null; then
        brew install googletest
    elif command -v apt-get &> /dev/null; then
        sudo apt-get install libgtest-dev
    else
        echo "Please install gtest manually"
        exit 1
    fi
fi

mkdir -p test/build
cd test/build

cmake -DCMAKE_BUILD_TYPE=Debug ..
make test_ant_mania

if [ $? -eq 0 ]; then
    echo ""
    ./test_ant_mania
    echo ""
    echo "Tests done!"
else
    echo "Build failed!"
    exit 1
fi