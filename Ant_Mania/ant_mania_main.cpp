#include "ant_mania.h"
#include <memory>
#include <iomanip>

void printUsage(const char* program_name) {
    std::cout << "Ant Mania Simulation - High Performance C++ Implementation" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Usage: " << program_name << " <map_file> <num_ants> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Required Arguments:" << std::endl;
    std::cout << "  <map_file>     Path to the colony map file" << std::endl;
    std::cout << "  <num_ants>     Number of ants to simulate" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --single       Use single-threaded simulation (default)" << std::endl;
    std::cout << "  --multi        Use lock-free multithreaded simulation" << std::endl;
    std::cout << "  --benchmark    Run performance benchmark comparison" << std::endl;
    std::cout << "  --runs N       Number of benchmark runs (default: 5)" << std::endl;
    std::cout << "  --help         Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " hiveum_map_small.txt 100" << std::endl;
    std::cout << "  " << program_name << " hiveum_map_medium.txt 1000 --multi" << std::endl;
    std::cout << "  " << program_name << " hiveum_map_small.txt 100 --benchmark" << std::endl;
    std::cout << "  " << program_name << " hiveum_map_medium.txt 1000 --benchmark --runs 10" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  • Structure of Arrays (SoA) for cache efficiency" << std::endl;
    std::cout << "  • Integer IDs instead of string lookups" << std::endl;
    std::cout << "  • Batch processing for better cache performance" << std::endl;
    std::cout << "  • Lock-free multithreading with atomic operations" << std::endl;
    std::cout << "  • Comprehensive benchmarking and performance analysis" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Check for help
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
    std::string map_file = argv[1];
    uint32_t num_ants = std::stoul(argv[2]);
    
    if (num_ants <= 0) {
        std::cerr << "Error: Number of ants must be positive" << std::endl;
        return 1;
    }
    
    // Parse options
    bool use_multithreading = false;
    bool run_benchmark = false;
    size_t num_runs = 5;
    
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--single") {
            use_multithreading = false;
        } else if (arg == "--multi") {
            use_multithreading = true;
        } else if (arg == "--benchmark") {
            run_benchmark = true;
        } else if (arg == "--runs" && i + 1 < argc) {
            num_runs = std::stoul(argv[++i]);
            if (num_runs <= 0) {
                std::cerr << "Error: Number of runs must be positive" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Run benchmark if requested
    if (run_benchmark) {
        std::cout << "Starting benchmark with " << num_runs << " runs..." << std::endl;
        BenchmarkRunner benchmark(map_file, num_ants, num_runs);
        benchmark.runComparison();
        return 0;
    }
    
    // Create simulation based on mode
    std::unique_ptr<AntManiaSimulation> simulation;
    
    if (use_multithreading) {
        simulation = std::make_unique<MultithreadedSimulation>();
        std::cout << "Using lock-free multithreaded simulation" << std::endl;
    } else {
        simulation = std::make_unique<SingleThreadedSimulation>();
        std::cout << "Using single-threaded simulation" << std::endl;
    }
    
    // Load map
    if (!simulation->loadMap(map_file)) {
        return 1;
    }
    
    // Create ants
    simulation->createAnts(num_ants);
    
    // Run simulation
    simulation->runSimulation();
    
    // Print results
    simulation->printStatistics();
    simulation->printRemainingWorld();
    
    return 0;
}
