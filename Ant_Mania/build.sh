#!/bin/bash

echo "Building..."
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4

echo "Done! Run with:"
echo "  ./ant_mania ../hiveum_map_small.txt 100"
echo "  ./benchmark ./ant_mania"