#!/bin/bash

echo "Building Ant Mania simulation..."
mkdir -p build_simple
cd build_simple

echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

echo "Building project..."
make -j$(sysctl -n hw.ncpu)

echo "Build completed successfully!"
echo ""
echo "Available executable:"
echo "  ./ant_mania - High performance simulation"
echo ""
echo "Usage examples:"
echo "  ./ant_mania ../hiveum_map_small.txt 50"
echo "  ./ant_mania ../hiveum_map_medium.txt 1000"
echo ""
echo "Performance features:"
echo "  • Structure of Arrays (SoA) for cache efficiency"
echo "  • Integer IDs instead of string lookups"
echo "  • Batch processing for better cache performance"
echo "  • Pre-allocated vectors to avoid dynamic allocation"
echo "  • Early termination conditions"
echo "  • Compiler optimizations (-O3, -march=native, -flto)"
