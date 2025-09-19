#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <array>

// Ant Mania Simulation - High Performance Implementation
// Key features:
// 1. Three-pass map parsing to resolve colony IDs correctly
// 2. Proper collision reporting with ant IDs
// 3. Efficient data structures (fixed-size arrays)
// 4. Efficient random number generation
// 5. Incremental termination checking
// 6. Correct fight counting
// 7. Zero allocations in hot path (checkCollisions)
// 8. Constexpr direction helpers with enum class

enum class Direction : uint8_t {
    NORTH = 0,
    SOUTH = 1,
    EAST = 2,
    WEST = 3,
    INVALID = 255
};

struct Ant {
    uint32_t colony_id;    // Current colony (integer ID)
    uint16_t move_count;   // Number of moves made
    bool alive;            // Is ant still alive
    uint32_t ant_id;       // Unique ant identifier for reporting
};

struct Colony {
    std::array<uint32_t, 4> connections;  // north, south, east, west -> colony IDs (UINT32_MAX = no connection)
    bool destroyed;                        // Is colony destroyed
};

class AntManiaSimulation {
private:
    // Core data structures - optimized for cache performance
    std::vector<Ant> ants;
    std::vector<Colony> colonies;
    
    // Lookup tables for string <-> ID conversion
    std::vector<std::string> colony_names;
    std::unordered_map<std::string, uint32_t> name_to_id;
    
    // Random number generation - multiple distributions for unbiased selection
    std::mt19937_64 rng;
    std::uniform_int_distribution<uint32_t> direction_dist;
    std::array<std::uniform_int_distribution<uint8_t>, 4> count_dists;  // For unbiased random selection
    
    // Statistics
    uint32_t total_ants;
    uint32_t colonies_destroyed;
    uint32_t total_fight_pairs;  // Total number of ant pairs that fought
    
    // Incremental counters for efficient termination checking
    uint32_t alive_ants_count;
    uint32_t max_moves_ants_count;
    
    // Reusable buffers to avoid allocations in hot path
    std::vector<uint32_t> colony_ant_counts;
    std::vector<std::array<uint32_t, 2>> colony_ant_ids;  // Store first 2 IDs per colony
    
    // Constants
    static constexpr uint32_t MAX_MOVES = 10000;
    static constexpr uint32_t NO_CONNECTION = UINT32_MAX;

    // Helper functions
    static Direction direction_to_enum(const std::string& direction);
    static constexpr const char* enum_to_direction(Direction dir);
    void moveAnts();
    void checkCollisions();

public:
    AntManiaSimulation();
    
    bool loadMap(const std::string& filename);
    void createAnts(uint32_t num_ants);
    void runSimulation();
    void printRemainingWorld();
    void printStatistics();
};
