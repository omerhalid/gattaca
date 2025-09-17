#include "ant_mania.h"

// Benchmarking implementation
BenchmarkRunner::BenchmarkResult BenchmarkRunner::runBenchmark(std::unique_ptr<AntManiaSimulation> simulation) {
    std::vector<double> times;
    uint32_t total_fights = 0;
    uint32_t colonies_destroyed = 0;
    uint32_t ants_remaining = 0;
    
    std::cout << "\n=== Running " << simulation->getModeName() << " Benchmark ===" << std::endl;
    std::cout << "Map: " << map_file << ", Ants: " << num_ants << ", Runs: " << num_runs << std::endl;
    
    for (size_t run = 0; run < num_runs; ++run) {
        // Create fresh simulation for each run
        std::unique_ptr<AntManiaSimulation> sim;
        if (simulation->getModeName() == "Single-threaded") {
            sim = std::make_unique<SingleThreadedSimulation>();
        } else {
            sim = std::make_unique<MultithreadedSimulation>();
        }
        
        // Load map and create ants
        if (!sim->loadMap(map_file)) {
            std::cerr << "Failed to load map for benchmark run " << run + 1 << std::endl;
            continue;
        }
        sim->createAnts(num_ants);
        
        // Run simulation and measure time
        auto start_time = std::chrono::high_resolution_clock::now();
        sim->runSimulation();
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        double time_ms = duration.count() / 1000.0;
        times.push_back(time_ms);
        
        // Collect statistics from last run
        if (run == num_runs - 1) {
            total_fights = sim->total_fights;
            colonies_destroyed = sim->colonies_destroyed;
            ants_remaining = sim->countAliveAnts();
        }
        
        std::cout << "Run " << run + 1 << ": " << time_ms << " ms" << std::endl;
    }
    
    // Calculate statistics
    double avg_time = 0.0;
    double min_time = times[0];
    double max_time = times[0];
    
    for (double time : times) {
        avg_time += time;
        min_time = std::min(min_time, time);
        max_time = std::max(max_time, time);
    }
    avg_time /= times.size();
    
    BenchmarkResult result;
    result.mode_name = simulation->getModeName();
    result.avg_time_ms = avg_time;
    result.min_time_ms = min_time;
    result.max_time_ms = max_time;
    result.total_fights = total_fights;
    result.colonies_destroyed = colonies_destroyed;
    result.ants_remaining = ants_remaining;
    
    std::cout << "\n" << result.mode_name << " Results:" << std::endl;
    std::cout << "  Average time: " << avg_time << " ms" << std::endl;
    std::cout << "  Min time: " << min_time << " ms" << std::endl;
    std::cout << "  Max time: " << max_time << " ms" << std::endl;
    std::cout << "  Total fights: " << total_fights << std::endl;
    std::cout << "  Colonies destroyed: " << colonies_destroyed << std::endl;
    std::cout << "  Ants remaining: " << ants_remaining << std::endl;
    
    return result;
}

void BenchmarkRunner::runComparison() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ANT MANIA PERFORMANCE COMPARISON" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Run single-threaded benchmark
    auto single_result = runBenchmark(std::make_unique<SingleThreadedSimulation>());
    
    // Run multithreaded benchmark
    auto multi_result = runBenchmark(std::make_unique<MultithreadedSimulation>());
    
    // Compare results
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "PERFORMANCE COMPARISON" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << std::left << std::setw(25) << "Metric" 
              << std::setw(20) << "Single-threaded" 
              << std::setw(20) << "Multithreaded" 
              << std::setw(15) << "Speedup" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    std::cout << std::left << std::setw(25) << "Average Time (ms)"
              << std::setw(20) << std::fixed << std::setprecision(2) << single_result.avg_time_ms
              << std::setw(20) << std::fixed << std::setprecision(2) << multi_result.avg_time_ms
              << std::setw(15) << std::fixed << std::setprecision(2) << (single_result.avg_time_ms / multi_result.avg_time_ms) << std::endl;
    
    std::cout << std::left << std::setw(25) << "Min Time (ms)"
              << std::setw(20) << std::fixed << std::setprecision(2) << single_result.min_time_ms
              << std::setw(20) << std::fixed << std::setprecision(2) << multi_result.min_time_ms
              << std::setw(15) << std::fixed << std::setprecision(2) << (single_result.min_time_ms / multi_result.min_time_ms) << std::endl;
    
    std::cout << std::left << std::setw(25) << "Max Time (ms)"
              << std::setw(20) << std::fixed << std::setprecision(2) << single_result.max_time_ms
              << std::setw(20) << std::fixed << std::setprecision(2) << multi_result.max_time_ms
              << std::setw(15) << std::fixed << std::setprecision(2) << (single_result.max_time_ms / multi_result.max_time_ms) << std::endl;
    
    std::cout << std::left << std::setw(25) << "Total Fights"
              << std::setw(20) << single_result.total_fights
              << std::setw(20) << multi_result.total_fights
              << std::setw(15) << (single_result.total_fights == multi_result.total_fights ? "✓" : "✗") << std::endl;
    
    std::cout << std::left << std::setw(25) << "Colonies Destroyed"
              << std::setw(20) << single_result.colonies_destroyed
              << std::setw(20) << multi_result.colonies_destroyed
              << std::setw(15) << (single_result.colonies_destroyed == multi_result.colonies_destroyed ? "✓" : "✗") << std::endl;
    
    std::cout << std::left << std::setw(25) << "Ants Remaining"
              << std::setw(20) << single_result.ants_remaining
              << std::setw(20) << multi_result.ants_remaining
              << std::setw(15) << (single_result.ants_remaining == multi_result.ants_remaining ? "✓" : "✗") << std::endl;
    
    double speedup = single_result.avg_time_ms / multi_result.avg_time_ms;
    std::cout << "\nOverall Speedup: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    
    if (speedup > 1.1) {
        std::cout << "Multithreaded version is " << speedup << "x faster!" << std::endl;
    } else if (speedup < 0.9) {
        std::cout << "Single-threaded version is " << (1.0 / speedup) << "x faster!" << std::endl;
    } else {
        std::cout << "Both versions perform similarly." << std::endl;
    }
    
    std::cout << std::string(60, '=') << std::endl;
}
