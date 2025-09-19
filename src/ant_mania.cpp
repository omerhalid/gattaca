#include "ant_mania.h"

AntManiaSimulation::AntManiaSimulation() 
    : rng(std::random_device{}())
    , direction_dist(0, 3)  // 0-3 for directions
    , count_dists{std::uniform_int_distribution<uint8_t>(0, 0),  // 1 direction
                  std::uniform_int_distribution<uint8_t>(0, 1),  // 2 directions
                  std::uniform_int_distribution<uint8_t>(0, 2),  // 3 directions
                  std::uniform_int_distribution<uint8_t>(0, 3)}  // 4 directions
    , total_ants(0)
    , colonies_destroyed(0)
    , total_fight_pairs(0)
    , alive_ants_count(0)
    , max_moves_ants_count(0) {}

bool AntManiaSimulation::loadMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    
    // First pass: count colonies and reserve memory
    std::string line;
    size_t colony_count = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) colony_count++;
    }
    
    // Reserve memory to avoid reallocations during simulation
    colonies.reserve(colony_count);
    colony_names.reserve(colony_count);
    name_to_id.reserve(colony_count);
    
    // Second pass: parse colony names first
    file.clear();
    file.seekg(0);
    
    uint32_t colony_id = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Parse colony name
        size_t space_pos = line.find(' ');
        std::string colony_name = line.substr(0, space_pos);
        
        // Store mapping
        colony_names.push_back(colony_name);
        name_to_id[colony_name] = colony_id;
        colony_id++;
    }
    
    // Third pass: parse connections with resolved IDs
    file.clear();
    file.seekg(0);
    
    colony_id = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Initialize colony with no connections
        Colony colony;
        colony.destroyed = false;
        colony.connections.fill(NO_CONNECTION);
        
        // Parse connections
        size_t space_pos = line.find(' ');
        std::istringstream iss(line.substr(space_pos + 1));
        std::string connection;
        
        while (iss >> connection) {
            size_t eq_pos = connection.find('=');
            if (eq_pos != std::string::npos) {
                std::string direction = connection.substr(0, eq_pos);
                std::string target = connection.substr(eq_pos + 1);
                
                // Convert direction to integer using constexpr helper
                Direction dir = direction_to_enum(direction);
                // Skip invalid directions explicitly
                if (dir != Direction::INVALID) {
                    // Now we can safely resolve the target ID
                    auto it = name_to_id.find(target);
                    if (it != name_to_id.end()) {
                        colony.connections[static_cast<uint8_t>(dir)] = it->second;
                    }
                }
            }
        }
        
        colonies.push_back(colony);
        colony_id++;
    }
    
    // Initialize reusable buffers
    colony_ant_counts.resize(colony_count, 0);
    colony_ant_ids.resize(colony_count, {UINT32_MAX, UINT32_MAX});
    
    std::cout << "Loaded " << colony_count << " colonies from " << filename << std::endl;
    return true;
}

void AntManiaSimulation::createAnts(uint32_t num_ants) {
    total_ants = num_ants;
    alive_ants_count = num_ants;
    max_moves_ants_count = 0;
    ants.reserve(num_ants);
    
    // Get available colonies (non-destroyed)
    std::vector<uint32_t> available_colonies;
    for (uint32_t i = 0; i < colonies.size(); ++i) {
        if (!colonies[i].destroyed) {
            available_colonies.push_back(i);
        }
    }
    
    if (available_colonies.empty()) {
        std::cerr << "Error: No available colonies for ants to start" << std::endl;
        return;
    }
    
    // Create ants at random positions
    std::uniform_int_distribution<uint32_t> colony_dist(0, available_colonies.size() - 1);
    
    for (uint32_t i = 0; i < num_ants; ++i) {
        Ant ant;
        ant.colony_id = available_colonies[colony_dist(rng)];
        ant.move_count = 0;
        ant.alive = true;
        ant.ant_id = i;  // Assign unique ID
        ants.push_back(ant);
    }
    
    std::cout << "Created " << num_ants << " ants" << std::endl;
}

void AntManiaSimulation::runSimulation() {
    std::cout << "Starting simulation..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    uint32_t iteration = 0;
    
    while (true) {
        iteration++;
        
        // Early termination check - now O(1) instead of O(n)
        if (alive_ants_count == 0 || alive_ants_count == max_moves_ants_count) {
            break;
        }
        
        // Move all ants
        moveAnts();
        
        // Check for collisions
        checkCollisions();
        
        // Progress reporting (less frequent for performance)
        if (iteration % 10000 == 0) {
            std::cout << "Iteration " << iteration << ": " << alive_ants_count << " ants alive, " 
                     << colonies_destroyed << " colonies destroyed" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "\nSimulation completed in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Total iterations: " << iteration << std::endl;
    std::cout << "Total fight pairs: " << total_fight_pairs << std::endl;
    std::cout << "Colonies destroyed: " << colonies_destroyed << std::endl;
    std::cout << "Ants remaining: " << alive_ants_count << std::endl;
}

void AntManiaSimulation::printRemainingWorld() {
    std::cout << "\nRemaining world map:" << std::endl;
    
    for (uint32_t i = 0; i < colonies.size(); ++i) {
        if (!colonies[i].destroyed) {
            std::cout << colony_names[i];
            
            // Print valid connections
            for (uint8_t dir = 0; dir < 4; ++dir) {
                if (colonies[i].connections[dir] != NO_CONNECTION && 
                    !colonies[colonies[i].connections[dir]].destroyed) {
                    std::cout << " " << enum_to_direction(static_cast<Direction>(dir)) << "=" << colony_names[colonies[i].connections[dir]];
                }
            }
            std::cout << std::endl;
        }
    }
}

void AntManiaSimulation::printStatistics() {
    std::cout << "\n=== Performance Statistics ===" << std::endl;
    std::cout << "Total ants created: " << total_ants << std::endl;
    std::cout << "Colonies destroyed: " << colonies_destroyed << std::endl;
    std::cout << "Total fight pairs: " << total_fight_pairs << std::endl;
    std::cout << "Ants remaining: " << alive_ants_count << std::endl;
    
    uint32_t remaining_colonies = 0;
    for (const auto& colony : colonies) {
        if (!colony.destroyed) {
            remaining_colonies++;
        }
    }
    std::cout << "Colonies remaining: " << remaining_colonies << std::endl;
}

Direction AntManiaSimulation::direction_to_enum(const std::string& direction) {
    switch (direction[0]) {
        case 'n': return Direction::NORTH;
        case 's': return Direction::SOUTH;
        case 'e': return Direction::EAST;
        case 'w': return Direction::WEST;
        default: return Direction::INVALID;  // Explicitly mark invalid directions
    }
}

constexpr const char* AntManiaSimulation::enum_to_direction(Direction dir) {
    switch (dir) {
        case Direction::NORTH: return "north";
        case Direction::SOUTH: return "south";
        case Direction::EAST: return "east";
        case Direction::WEST: return "west";
        case Direction::INVALID: return "invalid";
    }
    return "invalid";  // fallback
}

void AntManiaSimulation::moveAnts() {
    // Process ants in batches for better cache performance
    static constexpr size_t BATCH_SIZE = 8;
    
    for (size_t i = 0; i < ants.size(); i += BATCH_SIZE) {
        size_t batch_size = std::min(BATCH_SIZE, ants.size() - i);
        
        for (size_t j = 0; j < batch_size; ++j) {
            Ant& ant = ants[i + j];
            
            // Skip dead ants
            if (!ant.alive) continue;
            
            uint32_t current_colony = ant.colony_id;
            
            // Check if colony is destroyed
            if (colonies[current_colony].destroyed) {
                ant.alive = false;
                alive_ants_count--;
                continue;
            }
            
            // Find valid connections - using stack-allocated array
            uint8_t valid_dirs[4];
            uint8_t count = 0;
            for (uint8_t dir = 0; dir < 4; ++dir) {
                uint32_t target = colonies[current_colony].connections[dir];
                if (target != NO_CONNECTION && !colonies[target].destroyed) {
                    valid_dirs[count++] = dir;
                }
            }
            
            if (count == 0) {
                ant.alive = false;
                alive_ants_count--;
                continue;
            }
            
            // Move to random valid connection
            uint8_t random_dir = valid_dirs[count_dists[count - 1](rng)];
            ant.colony_id = colonies[current_colony].connections[random_dir];
            ant.move_count++;
            
            // Update max moves counter incrementally
            if (ant.move_count == MAX_MOVES) {
                max_moves_ants_count++;
            }
        }
    }
}

void AntManiaSimulation::checkCollisions() {
    // Clear reusable buffers
    std::fill(colony_ant_counts.begin(), colony_ant_counts.end(), 0);
    for (auto& ids : colony_ant_ids) {
        ids[0] = UINT32_MAX;
        ids[1] = UINT32_MAX;
    }
    
    // Count ants per colony and collect first 2 ant IDs
    for (const auto& ant : ants) {
        if (ant.alive) {
            uint32_t colony_id = ant.colony_id;
            colony_ant_counts[colony_id]++;
            
            // Store first 2 IDs for reporting
            if (colony_ant_ids[colony_id][0] == UINT32_MAX) {
                colony_ant_ids[colony_id][0] = ant.ant_id;
            } else if (colony_ant_ids[colony_id][1] == UINT32_MAX) {
                colony_ant_ids[colony_id][1] = ant.ant_id;
            }
        }
    }
    
    // Check for collisions (2+ ants in same colony)
    for (uint32_t colony_id = 0; colony_id < colonies.size(); ++colony_id) {
        if (colony_ant_counts[colony_id] >= 2 && !colonies[colony_id].destroyed) {
            // Destroy colony
            colonies[colony_id].destroyed = true;
            colonies_destroyed++;
            
            // Count fight pairs - each pair of ants is a fight
            uint32_t ants_in_colony = colony_ant_counts[colony_id];
            total_fight_pairs += (ants_in_colony * (ants_in_colony - 1)) / 2;
            
            // Kill all ants in this colony and update counters
            for (auto& ant : ants) {
                if (ant.alive && ant.colony_id == colony_id) {
                    ant.alive = false;
                    alive_ants_count--;
                }
            }
            
            // Report collision with first 2 ant IDs as required by spec
            std::cout << colony_names[colony_id] << " has been destroyed by ant " 
                     << colony_ant_ids[colony_id][0] << " and ant " 
                     << colony_ant_ids[colony_id][1] << "!" << std::endl;
        }
    }
}