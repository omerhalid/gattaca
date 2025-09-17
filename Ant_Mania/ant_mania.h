#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <memory_resource>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>

// Structure of Arrays (SoA) for cache efficiency
struct AntData {
    std::vector<uint32_t> colony_ids;      // Current colony ID
    std::vector<uint16_t> move_counts;     // Move count per ant
    std::vector<uint8_t> alive_flags;      // Boolean packed as uint8_t
    std::vector<uint8_t> connection_count; // Pre-computed valid connections
    
    void reserve(size_t capacity) {
        colony_ids.reserve(capacity);
        move_counts.reserve(capacity);
        alive_flags.reserve(capacity);
        connection_count.reserve(capacity);
    }
    
    void resize(size_t size) {
        colony_ids.resize(size);
        move_counts.resize(size, 0);
        alive_flags.resize(size, 1);
        connection_count.resize(size, 0);
    }
};

// Colony data structure with integer IDs
struct ColonyData {
    std::vector<uint32_t> connections[4];  // 4 directions, each with target colony IDs
    std::vector<uint8_t> destroyed_flags;  // Boolean packed
    std::vector<uint8_t> connection_counts; // Pre-computed connection count
    
    void reserve(size_t capacity) {
        for (int i = 0; i < 4; ++i) {
            connections[i].reserve(capacity);
        }
        destroyed_flags.reserve(capacity);
        connection_counts.reserve(capacity);
    }
    
    void resize(size_t size) {
        for (int i = 0; i < 4; ++i) {
            connections[i].resize(size, UINT32_MAX); // Invalid ID
        }
        destroyed_flags.resize(size, 0);
        connection_counts.resize(size, 0);
    }
};

// Base class for Ant Mania simulation
class AntManiaSimulation {
protected:
    // Core data structures
    AntData ants;
    ColonyData colonies;
    
    // Pre-computed lookup tables
    std::vector<std::string> colony_names;  // ID -> name mapping
    std::unordered_map<std::string, uint32_t> name_to_id;  // name -> ID mapping
    
    // Random number generation
    std::mt19937_64 rng;
    
    // Constants
    static constexpr uint32_t MAX_MOVES = 10000;
    static constexpr size_t BATCH_WIDTH = 8;

public:
    // Statistics (public for benchmarking access)
    uint32_t total_ants = 0;
    uint32_t colonies_destroyed = 0;
    uint32_t total_fights = 0;
    AntManiaSimulation() : rng(std::random_device{}()) {}
    virtual ~AntManiaSimulation() = default;
    
    // Common functionality - implemented in base class
    bool loadMap(const std::string& filename);
    void createAnts(uint32_t num_ants);
    void printRemainingWorld();
    void printStatistics();
    uint32_t countAliveAnts();  // Public for benchmarking access
    
    // Virtual methods to be implemented by derived classes
    virtual void runSimulation() = 0;
    virtual std::string getModeName() const = 0;
    
    // Virtual method for ant movement (called thousands of times - performance critical)
    virtual void moveAnts() = 0;
    
protected:
    // Common helper methods
    int direction_to_int(const std::string& direction);
    std::string int_to_direction(int dir);
    bool checkTerminationCondition();
    void checkCollisionsBatch();
};

// Single-threaded implementation
class SingleThreadedSimulation : public AntManiaSimulation {
public:
    void runSimulation() override;
    std::string getModeName() const override { return "Single-threaded"; }
    
protected:
    void moveAnts() override;
    void moveAntsBatch();
};

// Lock-free multithreaded implementation
class MultithreadedSimulation : public AntManiaSimulation {
private:
    // Threading configuration
    static constexpr size_t MAX_THREADS = 4;
    
    // Synchronization for collision detection (only operation that needs locking)
    std::mutex collision_mutex;
    
    // Thread-local data to avoid contention
    struct ThreadLocalData {
        std::vector<uint32_t> local_colony_counts;
        std::mt19937_64 rng;
        
        ThreadLocalData(size_t colony_count) 
            : local_colony_counts(colony_count, 0)
            , rng(std::random_device{}()) {}
    };

public:
    void runSimulation() override;
    std::string getModeName() const override { return "Lock-free Multithreaded"; }
    
protected:
    void moveAnts() override;
    void moveAntsMultithreaded();
    void moveAntsBatch(uint32_t start_ant, uint32_t end_ant);
    void checkCollisionsBatchLockFree();
};

// Benchmarking class
class BenchmarkRunner {
private:
    std::string map_file;
    uint32_t num_ants;
    size_t num_runs;
    
public:
    BenchmarkRunner(const std::string& map_file, uint32_t num_ants, size_t num_runs = 5)
        : map_file(map_file), num_ants(num_ants), num_runs(num_runs) {}
    
    struct BenchmarkResult {
        std::string mode_name;
        double avg_time_ms;
        double min_time_ms;
        double max_time_ms;
        uint32_t total_fights;
        uint32_t colonies_destroyed;
        uint32_t ants_remaining;
    };
    
    BenchmarkResult runBenchmark(std::unique_ptr<AntManiaSimulation> simulation);
    void runComparison();
};
