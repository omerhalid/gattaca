#!/bin/bash

echo "Building Ant Mania simulation..."
mkdir -p build
cd build

echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

echo "Building project..."
make -j$(sysctl -n hw.ncpu)

echo "Build completed successfully!"
echo ""
echo "Available executables:"
echo "  ./ant_mania - High performance simulation"
echo "  ./benchmark - Automated benchmark tool"
echo ""
echo "Usage examples:"
echo "  ./ant_mania ../hiveum_map_small.txt 50"
echo "  ./ant_mania ../hiveum_map_medium.txt 1000"
echo "  ./benchmark ./ant_mania"
echo "  ./benchmark ./ant_mania ../hiveum_map_small.txt 50 100 200"
echo ""
echo "Performance features:"
echo "  • Array of Structures (AoS) for optimal cache performance"
echo "  • Integer IDs instead of string lookups"
echo "  • Batch processing for better cache performance"
echo "  • Pre-allocated vectors to avoid dynamic allocation"
echo "  • Early termination conditions"
echo "  • Compiler optimizations (-O3, -march=native, -flto)"
