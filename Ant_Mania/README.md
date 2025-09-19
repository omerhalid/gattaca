# Ant Mania - High Performance C++ Implementation

## Overview
High-performance simulation of space ant invasion on planet Hiveum, optimized for **simulation latency** as the primary requirement.

## Key Features
- **High Performance**: Sub-100ms execution for complex scenarios
- **Professional Architecture**: Clean separation of header/implementation files
- **Optimized Algorithms**: Efficient collision detection and ant movement
- **Memory Efficient**: Zero allocations in hot paths, cache-friendly data structures

## Performance Results

### Small Map 
- **50 ants**: ~14ms (many iterations due to colony destruction effect)
- **100 ants**: ~8ms 
- **200 ants**: ~7ms

### Medium Map 
- **100-2000 ants**: ~75-80ms consistently
- **Excellent scalability**: Performance driven by iterations, not just ant count

## Build & Run

```bash
# Build project
./build.sh

# Run simulation
cd build
./ant_mania ../hiveum_map_small.txt 100
./ant_mania ../hiveum_map_medium.txt 1000

# Run automated benchmarks
./benchmark ./ant_mania
./benchmark ./ant_mania ../hiveum_map_small.txt 50 100 200

# Interactive benchmark runner
../run_benchmark.sh
```

## Benchmark Tool

Simple automated benchmark tool (`benchmark.cpp`):
- **Automated testing**: Multiple simulations with different parameters
- **Performance metrics**: Execution time measurements
- **Clean output**: Formatted results
- **Flexible configuration**: Custom map files and ant counts

## Architecture

**Professional modular design**:
- `ant_mania.h`: Header file with class declarations and data structures
- `ant_mania.cpp`: Implementation of the core simulation logic  
- `main.cpp`: Entry point and command-line interface
- `AntManiaSimulation`: Main simulation class with optimized algorithms

## Documentation

For detailed optimization analysis and benchmark results, see:
- **`optimization_doc.md`**: Complete optimization report with performance analysis

## Requirements

- **C++17** or later
- **CMake 3.16** or later
- **Compiler**: GCC, Clang, or MSVC with optimization support
