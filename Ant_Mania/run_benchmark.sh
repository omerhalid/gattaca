#!/bin/bash

echo "Ant Mania Benchmark Runner"
echo "========================="
echo ""

# Check if we're in the right directory
if [ ! -f "build/ant_mania" ] || [ ! -f "build/benchmark" ]; then
    echo "Error: Build directory not found or executables not built."
    echo "Please run './build.sh' first."
    exit 1
fi

cd build

echo "Available benchmark options:"
echo "1. Quick test (small map, 50-200 ants)"
echo "2. Medium test (medium map, 100-2000 ants)" 
echo "3. Full benchmark suite"
echo "4. Custom test"
echo ""

read -p "Select option (1-4): " choice

case $choice in
    1)
        echo "Running quick test..."
        ./benchmark ./ant_mania ../hiveum_map_small.txt 50 100 200
        ;;
    2)
        echo "Running medium test..."
        ./benchmark ./ant_mania ../hiveum_map_medium.txt 100 500 1000 2000
        ;;
    3)
        echo "Running full benchmark suite..."
        ./benchmark ./ant_mania
        ;;
    4)
        echo "Custom benchmark"
        read -p "Enter map file (e.g., ../hiveum_map_small.txt): " map_file
        read -p "Enter ant counts (space-separated, e.g., 50 100 500): " ant_counts
        
        if [ -z "$ant_counts" ]; then
            ant_counts="50 100 500"
        fi
        
        echo "Running custom test..."
        ./benchmark ./ant_mania "$map_file" $ant_counts
        ;;
    *)
        echo "Invalid option. Running full benchmark suite..."
        ./benchmark ./ant_mania
        ;;
esac

echo ""
echo "Benchmark completed!"
