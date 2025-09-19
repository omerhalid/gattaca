#!/bin/bash

echo "Building..."
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4

echo "Done! Run with:"
echo "  ./ant_mania ../task/hiveum_map_small.txt 100"
echo "  ./ant_mania ../task/hiveum_map_medium.txt 1000"
echo "  ./benchmark ./ant_mania"