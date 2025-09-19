#!/bin/bash

echo "Benchmark runner"
echo ""

if [ ! -f "build/ant_mania" ] || [ ! -f "build/benchmark" ]; then
    echo "Build first with ./build.sh"
    exit 1
fi

cd build

echo "Options:"
echo "1. Quick test"
echo "2. Medium test" 
echo "3. Full test"
echo ""

read -p "Pick (1-3): " choice

case $choice in
    1)
        ./benchmark ./ant_mania ../hiveum_map_small.txt 50 100 200
        ;;
    2)
        ./benchmark ./ant_mania ../hiveum_map_medium.txt 100 500 1000
        ;;
    3)
        ./benchmark ./ant_mania
        ;;
    *)
        ./benchmark ./ant_mania
        ;;
esac

echo "Done!"