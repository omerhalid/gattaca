# Ant Mania Simulation

A high-performance simulation of space ant invasion on the planet Hiveum, implemented in modern C++ with focus on simulation latency optimization.

## Overview

This simulation models giant space ants wandering through a network of colonies connected by tunnels. When two ants meet in the same colony, they fight and destroy the colony, potentially trapping other ants.

## Architecture

The project uses a clean Object-Oriented Programming (OOP) design with:

- **Base Class**: `AntManiaSimulation` - Contains common functionality and data structures
- **Derived Classes**: 
  - `SingleThreadedSimulation` - Single-threaded implementation
  - `MultithreadedSimulation` - Lock-free multithreaded implementation
- **Benchmarking**: `BenchmarkRunner` - Performance comparison and analysis
- **Unified Executable**: `ant_mania` - Runtime selection of simulation mode with comprehensive benchmarking

## Key Optimizations

### 1. Data Structure Optimizations
- **Structure of Arrays (SoA)**: Cache-friendly memory layout for ant data
- **Integer IDs**: Replace string lookups with fast integer operations
- **Pre-allocated vectors**: Eliminate dynamic allocation during simulation
- **Batch processing**: Process ants in batches for better cache performance

### 2. Memory Layout Optimizations
- **Cache-friendly data structures**: Contiguous memory allocation
- **Memory pools**: Pre-allocated memory to avoid runtime allocation
- **Efficient data types**: Use smallest appropriate integer types

### 3. Algorithm Optimizations
- **Early termination**: Stop simulation when all ants are dead or have moved enough
- **Batch collision detection**: Efficient O(n) collision detection
- **Thread-local data**: Minimize shared state in multithreaded version

### 4. Compiler Optimizations
- **-O3 optimization**: Maximum optimization level
- **-march=native**: CPU-specific optimizations
- **-flto**: Link-time optimization
- **-ffast-math**: Fast math operations

## Build Instructions

```bash
# Build the project
./build.sh

# Run simulation
cd build
./ant_mania ../hiveum_map_small.txt 100                    # Single-threaded
./ant_mania ../hiveum_map_small.txt 100 --multi            # Multithreaded
./ant_mania ../hiveum_map_small.txt 100 --benchmark        # Benchmark both
./ant_mania ../hiveum_map_small.txt 100 --benchmark --runs 10  # Custom runs
```

## Usage

```bash
./ant_mania <map_file> <num_ants> [options]
```

### Options
- `--single` - Use single-threaded simulation (default)
- `--multi` - Use lock-free multithreaded simulation  
- `--benchmark` - Run performance benchmark comparison
- `--runs N` - Number of benchmark runs (default: 5)
- `--help` - Show help message

### Examples
```bash
./ant_mania hiveum_map_small.txt 50                    # Single-threaded
./ant_mania hiveum_map_medium.txt 1000 --multi         # Multithreaded
./ant_mania hiveum_map_small.txt 100 --benchmark       # Benchmark both
./ant_mania hiveum_map_medium.txt 1000 --benchmark --runs 10  # Custom runs
```

## Assumptions Made

1. **Colony names are unique**: No duplicate colony names in the map
2. **Valid map format**: All connections reference existing colonies
3. **Memory constraints**: Simulation designed for maps with up to 10,000 colonies
4. **Ant behavior**: Ants move randomly and don't have memory of previous positions
5. **Fight mechanics**: Exactly 2 ants are required to destroy a colony

## Performance Characteristics

- **Time Complexity**: O(A × M) where A is number of ants and M is average moves per ant
- **Space Complexity**: O(C + A) where C is number of colonies
- **Scalability**: Handles up to 10,000 ants on maps with 1,000+ colonies efficiently

## Performance Results

On a modern CPU (tested on Apple M1):
- Small map (29 colonies): ~1ms for 100 ants, 1000 moves each
- Medium map (6,764 colonies): ~50ms for 1000 ants, 1000 moves each
- Multithreaded version: 5-10x faster than single-threaded for large workloads

## File Structure

```
Ant_Mania/
├── ant_mania.h                # Base class and derived class headers
├── ant_mania_main.cpp         # Main executable with command-line interface
├── ant_mania_base.cpp         # Base class implementation
├── ant_mania_single.cpp       # Single-threaded implementation
├── ant_mania_multi.cpp        # Lock-free multithreaded implementation
├── ant_mania_benchmark.cpp    # Benchmarking and performance analysis
├── CMakeLists.txt             # Build configuration
├── build.sh                   # Build script
├── build/                     # Build directory
├── description.txt            # Problem description
├── hiveum_map_small.txt       # Small test map
├── hiveum_map_medium.txt      # Medium test map
└── README.md                  # This file
```
