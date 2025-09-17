#include "ant_mania.h"

// Base class implementation
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
    
    // Reserve memory to avoid reallocations
    colonies.reserve(colony_count);
    colony_names.reserve(colony_count);
    name_to_id.reserve(colony_count);
    
    // Second pass: parse and load data
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
        
        // Parse connections
        std::istringstream iss(line.substr(space_pos + 1));
        std::string connection;
        uint8_t connection_count = 0;
        
        while (iss >> connection) {
            size_t eq_pos = connection.find('=');
            if (eq_pos != std::string::npos) {
                std::string direction = connection.substr(0, eq_pos);
                std::string target = connection.substr(eq_pos + 1);
                
                // Convert direction to integer
                int dir = direction_to_int(direction);
                if (dir >= 0 && dir < 4) {
                    // Store target colony ID (will be resolved later)
                    colonies.connections[dir].push_back(name_to_id[target]);
                    connection_count++;
                }
            }
        }
        
        colonies.connection_counts.push_back(connection_count);
        colonies.destroyed_flags.push_back(0);
        colony_id++;
    }
    
    // Resize all vectors to final size
    colonies.resize(colony_count);
    
    std::cout << "Loaded " << colony_count << " colonies from " << filename << std::endl;
    return true;
}

void AntManiaSimulation::createAnts(uint32_t num_ants) {
    total_ants = num_ants;
    ants.reserve(num_ants);
    ants.resize(num_ants);
    
    // Get available colonies (non-destroyed)
    std::vector<uint32_t> available_colonies;
    available_colonies.reserve(colony_names.size());
    
    for (uint32_t i = 0; i < colony_names.size(); ++i) {
        if (!colonies.destroyed_flags[i]) {
            available_colonies.push_back(i);
        }
    }
    
    if (available_colonies.empty()) {
        std::cerr << "Error: No available colonies for ants to start" << std::endl;
        return;
    }
    
    // Batch random number generation
    std::uniform_int_distribution<uint32_t> colony_dist(0, available_colonies.size() - 1);
    
    // Generate random positions in batches
    for (uint32_t i = 0; i < num_ants; i += BATCH_WIDTH) {
        size_t batch_size = std::min(static_cast<size_t>(BATCH_WIDTH), static_cast<size_t>(num_ants - i));
        
        // Generate random numbers in batch
        for (size_t j = 0; j < batch_size; ++j) {
            uint32_t random_colony = colony_dist(rng);
            ants.colony_ids[i + j] = available_colonies[random_colony];
        }
    }
    
    std::cout << "Created " << num_ants << " ants" << std::endl;
}

void AntManiaSimulation::printRemainingWorld() {
    std::cout << "\nRemaining world map:" << std::endl;
    
    for (uint32_t i = 0; i < colony_names.size(); ++i) {
        if (!colonies.destroyed_flags[i]) {
            std::cout << colony_names[i];
            
            // Print valid connections
            for (int dir = 0; dir < 4; ++dir) {
                uint32_t target = colonies.connections[dir][i];
                if (target != UINT32_MAX && !colonies.destroyed_flags[target]) {
                    std::cout << " " << int_to_direction(dir) << "=" << colony_names[target];
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
    std::cout << "Total fights: " << total_fights << std::endl;
    
    uint32_t alive_count = countAliveAnts();
    std::cout << "Ants remaining: " << alive_count << std::endl;
    
    uint32_t remaining_colonies = 0;
    for (uint32_t i = 0; i < colony_names.size(); ++i) {
        if (!colonies.destroyed_flags[i]) {
            remaining_colonies++;
        }
    }
    std::cout << "Colonies remaining: " << remaining_colonies << std::endl;
}

// Helper methods
int AntManiaSimulation::direction_to_int(const std::string& direction) {
    switch (direction[0]) {
        case 'n': return 0;  // north
        case 's': return 1;  // south
        case 'e': return 2;  // east
        case 'w': return 3;  // west
        default: return -1;
    }
}

std::string AntManiaSimulation::int_to_direction(int dir) {
    static const std::array<std::string, 4> directions = {"north", "south", "east", "west"};
    return directions[dir];
}

bool AntManiaSimulation::checkTerminationCondition() {
    uint32_t alive_count = 0;
    uint32_t moved_enough_count = 0;
    
    // Process in batches
    for (uint32_t i = 0; i < total_ants; i += BATCH_WIDTH) {
        size_t batch_size = std::min(static_cast<size_t>(BATCH_WIDTH), static_cast<size_t>(total_ants - i));
        
        for (size_t j = 0; j < batch_size; ++j) {
            uint32_t idx = i + j;
            if (ants.alive_flags[idx]) {
                alive_count++;
                if (ants.move_counts[idx] >= MAX_MOVES) {
                    moved_enough_count++;
                }
            }
        }
    }
    
    return (alive_count == 0) || (alive_count == moved_enough_count);
}

uint32_t AntManiaSimulation::countAliveAnts() {
    uint32_t count = 0;
    for (uint32_t i = 0; i < total_ants; ++i) {
        count += ants.alive_flags[i];
    }
    return count;
}

void AntManiaSimulation::checkCollisionsBatch() {
    // Use counting sort for O(n) collision detection
    std::vector<uint32_t> colony_ant_counts(colony_names.size(), 0);
    
    // Count ants per colony
    for (uint32_t i = 0; i < total_ants; ++i) {
        if (ants.alive_flags[i]) {
            colony_ant_counts[ants.colony_ids[i]]++;
        }
    }
    
    // Check for collisions (2+ ants in same colony)
    for (uint32_t colony_id = 0; colony_id < colony_names.size(); ++colony_id) {
        if (colony_ant_counts[colony_id] >= 2 && !colonies.destroyed_flags[colony_id]) {
            // Destroy colony
            colonies.destroyed_flags[colony_id] = 1;
            colonies_destroyed++;
            total_fights++;
            
            // Kill all ants in this colony
            for (uint32_t i = 0; i < total_ants; ++i) {
                if (ants.alive_flags[i] && ants.colony_ids[i] == colony_id) {
                    ants.alive_flags[i] = 0;
                }
            }
            
            std::cout << colony_names[colony_id] << " has been destroyed!" << std::endl;
        }
    }
}
