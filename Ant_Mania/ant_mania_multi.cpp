#include "ant_mania.h"

// Lock-free multithreaded implementation
void MultithreadedSimulation::runSimulation() {
    uint32_t iteration = 0;
    
    std::cout << "Starting lock-free multithreaded simulation..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    while (true) {
        iteration++;
        
        // Early termination check
        if (checkTerminationCondition()) {
            break;
        }
        
        // Multithreaded ant movement
        moveAnts();
        
        // Lock-free collision detection
        checkCollisionsBatchLockFree();
        
        // Progress reporting (less frequent for performance)
        if (iteration % 10000 == 0) {
            uint32_t alive_count = countAliveAnts();
            std::cout << "Iteration " << iteration << ": " << alive_count << " ants alive, " 
                     << colonies_destroyed << " colonies destroyed" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // Statistics are already updated in the base class
    
    std::cout << "\nLock-free multithreaded simulation completed in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Total iterations: " << iteration << std::endl;
    std::cout << "Total fights: " << total_fights << std::endl;
    std::cout << "Colonies destroyed: " << colonies_destroyed << std::endl;
    
    uint32_t alive_count = countAliveAnts();
    std::cout << "Ants remaining: " << alive_count << std::endl;
}

void MultithreadedSimulation::moveAnts() {
    moveAntsMultithreaded();
}

void MultithreadedSimulation::moveAntsMultithreaded() {
    // Determine thread count
    size_t num_threads = std::min(static_cast<size_t>(MAX_THREADS), static_cast<size_t>(std::thread::hardware_concurrency()));
    
    if (num_threads <= 1) {
        // Fallback to single-threaded
        moveAntsBatch(0, total_ants);
        return;
    }
    
    std::vector<std::thread> threads;
    uint32_t ants_per_thread = total_ants / num_threads;
    
    for (size_t i = 0; i < num_threads; ++i) {
        uint32_t start_ant = i * ants_per_thread;
        uint32_t end_ant = (i == num_threads - 1) ? total_ants : (i + 1) * ants_per_thread;
        
        threads.emplace_back([this, start_ant, end_ant]() {
            moveAntsBatch(start_ant, end_ant);
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
}

void MultithreadedSimulation::moveAntsBatch(uint32_t start_ant, uint32_t end_ant) {
    std::mt19937_64 local_rng(std::random_device{}());
    
    for (uint32_t i = start_ant; i < end_ant; ++i) {
        // Early exit for dead ants
        if (!ants.alive_flags[i]) continue;
        
        uint32_t current_colony = ants.colony_ids[i];
        
        // Check if colony is destroyed (read-only access)
        if (colonies.destroyed_flags[current_colony]) {
            ants.alive_flags[i] = false;
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
            ants.alive_flags[i] = false;
            continue;
        }
        
        // Move to random valid connection
        std::uniform_int_distribution<uint32_t> conn_dist(0, valid_connections - 1);
        ants.colony_ids[i] = valid_targets[conn_dist(local_rng)];
        ants.move_counts[i]++;
    }
}

void MultithreadedSimulation::checkCollisionsBatchLockFree() {
    // Use mutex for collision detection to avoid race conditions
    std::lock_guard<std::mutex> lock(collision_mutex);
    
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
