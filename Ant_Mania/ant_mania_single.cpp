#include "ant_mania.h"

// Single-threaded implementation
void SingleThreadedSimulation::runSimulation() {
    uint32_t iteration = 0;
    
    std::cout << "Starting single-threaded simulation..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    while (true) {
        iteration++;
        
        // Early termination check
        if (checkTerminationCondition()) {
            break;
        }
        
        // Move all ants
        moveAnts();
        
        // Check for collisions
        checkCollisionsBatch();
        
        // Progress reporting (less frequent for performance)
        if (iteration % 10000 == 0) {
            uint32_t alive_count = countAliveAnts();
            std::cout << "Iteration " << iteration << ": " << alive_count << " ants alive, " 
                     << colonies_destroyed << " colonies destroyed" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "\nSingle-threaded simulation completed in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Total iterations: " << iteration << std::endl;
    std::cout << "Total fights: " << total_fights << std::endl;
    std::cout << "Colonies destroyed: " << colonies_destroyed << std::endl;
    
    uint32_t alive_count = countAliveAnts();
    std::cout << "Ants remaining: " << alive_count << std::endl;
}

void SingleThreadedSimulation::moveAnts() {
    moveAntsBatch();
}

void SingleThreadedSimulation::moveAntsBatch() {
    // Process ants in batches
    for (uint32_t i = 0; i < total_ants; i += BATCH_WIDTH) {
        size_t batch_size = std::min(static_cast<size_t>(BATCH_WIDTH), static_cast<size_t>(total_ants - i));
        
        for (size_t j = 0; j < batch_size; ++j) {
            uint32_t idx = i + j;
            
            // Early exit for dead ants
            if (!ants.alive_flags[idx]) continue;
            
            uint32_t current_colony = ants.colony_ids[idx];
            
            // Check if colony is destroyed
            if (colonies.destroyed_flags[current_colony]) {
                ants.alive_flags[idx] = 0;
                continue;
            }
            
            // Find valid connections
            uint32_t valid_connections = 0;
            uint32_t valid_targets[4];
            
            for (int dir = 0; dir < 4; ++dir) {
                uint32_t target = colonies.connections[dir][current_colony];
                if (target != UINT32_MAX && !colonies.destroyed_flags[target]) {
                    valid_targets[valid_connections++] = target;
                }
            }
            
            if (valid_connections == 0) {
                ants.alive_flags[idx] = 0;
                continue;
            }
            
            // Move to random valid connection
            std::uniform_int_distribution<uint32_t> conn_dist(0, valid_connections - 1);
            ants.colony_ids[idx] = valid_targets[conn_dist(rng)];
            ants.move_counts[idx]++;
        }
    }
}
