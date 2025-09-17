#!/bin/bash

echo "Building Ant Mania simulation..."
mkdir -p build
cd build

echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "Building project..."
make -j$(sysctl -n hw.ncpu)

echo "Build completed successfully!"
echo ""
echo "Available executable:"
echo "  ./ant_mania - OOP-based simulation with benchmarking"
echo ""
echo "Usage examples:"
echo "  ./ant_mania ../hiveum_map_medium.txt 1000                    # Single-threaded"
echo "  ./ant_mania ../hiveum_map_medium.txt 1000 --multi            # Multithreaded"
echo "  ./ant_mania ../hiveum_map_medium.txt 1000 --benchmark        # Benchmark both"
echo "  ./ant_mania ../hiveum_map_medium.txt 1000 --benchmark --runs 10  # Custom runs"
