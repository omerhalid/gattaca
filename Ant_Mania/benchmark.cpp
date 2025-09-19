#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

struct BenchmarkResult {
    std::string map_file;
    int num_ants;
    double execution_time_ms;
    int total_iterations;
    int total_fight_pairs;
    int colonies_destroyed;
    int ants_remaining;
    int total_colonies;
};

class BenchmarkRunner {
private:
    std::string executable_path;
    
public:
    BenchmarkRunner(const std::string& exec_path) : executable_path(exec_path) {}
    
    BenchmarkResult runBenchmark(const std::string& map_file, int num_ants) {
        BenchmarkResult result;
        result.map_file = map_file;
        result.num_ants = num_ants;
        
        // Build command
        std::stringstream cmd;
        cmd << executable_path << " " << map_file << " " << num_ants;
        
        // Record start time
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Execute simulation
        FILE* pipe = popen(cmd.str().c_str(), "r");
        if (!pipe) {
            std::cerr << "Error: Could not execute command: " << cmd.str() << std::endl;
            return result;
        }
        
        // Parse output
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string line(buffer);
            
            // Extract statistics
            if (line.find("Simulation completed in") != std::string::npos) {
                // Extract microseconds
                size_t start = line.find("in ") + 3;
                size_t end = line.find(" microseconds");
                if (start != std::string::npos && end != std::string::npos) {
                    std::string time_str = line.substr(start, end - start);
                    result.execution_time_ms = std::stod(time_str) / 1000.0; // Convert to milliseconds
                }
            }
            else if (line.find("Total iterations:") != std::string::npos) {
                size_t start = line.find(": ") + 2;
                result.total_iterations = std::stoi(line.substr(start));
            }
            else if (line.find("Total fight pairs:") != std::string::npos) {
                size_t start = line.find(": ") + 2;
                result.total_fight_pairs = std::stoi(line.substr(start));
            }
            else if (line.find("Colonies destroyed:") != std::string::npos) {
                size_t start = line.find(": ") + 2;
                result.colonies_destroyed = std::stoi(line.substr(start));
            }
            else if (line.find("Ants remaining:") != std::string::npos) {
                size_t start = line.find(": ") + 2;
                result.ants_remaining = std::stoi(line.substr(start));
            }
        }
        
        pclose(pipe);
        
        // Calculate total colonies (approximate from map file)
        result.total_colonies = estimateColonyCount(map_file);
        
        return result;
    }
    
private:
    int estimateColonyCount(const std::string& map_file) {
        std::ifstream file(map_file);
        if (!file.is_open()) return 0;
        
        int count = 0;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) count++;
        }
        return count;
    }
};

void printHeader() {
    std::cout << std::string(120, '=') << std::endl;
    std::cout << "                           ANT MANIA SIMULATION BENCHMARK" << std::endl;
    std::cout << std::string(120, '=') << std::endl;
    std::cout << std::left
              << std::setw(20) << "Map File"
              << std::setw(10) << "Ants"
              << std::setw(15) << "Time (ms)"
              << std::setw(12) << "Iterations"
              << std::setw(15) << "Fight Pairs"
              << std::setw(15) << "Colonies Dest."
              << std::setw(12) << "Ants Left"
              << std::setw(15) << "Total Colonies"
              << std::endl;
    std::cout << std::string(120, '-') << std::endl;
}

void printResult(const BenchmarkResult& result) {
    std::cout << std::left
              << std::setw(20) << result.map_file
              << std::setw(10) << result.num_ants
              << std::setw(15) << std::fixed << std::setprecision(2) << result.execution_time_ms
              << std::setw(12) << result.total_iterations
              << std::setw(15) << result.total_fight_pairs
              << std::setw(15) << result.colonies_destroyed
              << std::setw(12) << result.ants_remaining
              << std::setw(15) << result.total_colonies
              << std::endl;
}

void printSummary(const std::vector<BenchmarkResult>& results) {
    std::cout << std::string(120, '=') << std::endl;
    std::cout << "                                    SUMMARY" << std::endl;
    std::cout << std::string(120, '=') << std::endl;
    
    double total_time = 0;
    int total_fights = 0;
    int total_destroyed = 0;
    
    for (const auto& result : results) {
        total_time += result.execution_time_ms;
        total_fights += result.total_fight_pairs;
        total_destroyed += result.colonies_destroyed;
    }
    
    std::cout << "Total simulations run: " << results.size() << std::endl;
    std::cout << "Average execution time: " << std::fixed << std::setprecision(2) 
              << (total_time / results.size()) << " ms" << std::endl;
    std::cout << "Total fight pairs: " << total_fights << std::endl;
    std::cout << "Total colonies destroyed: " << total_destroyed << std::endl;
    std::cout << std::string(120, '=') << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <executable_path> [map_file] [ant_counts...]" << std::endl;
        std::cout << "Example: " << argv[0] << " ./ant_mania ../hiveum_map_small.txt 50 100 500" << std::endl;
        std::cout << "Example: " << argv[0] << " ./ant_mania" << std::endl;
        return 1;
    }
    
    std::string executable = argv[1];
    std::vector<BenchmarkResult> results;
    
    // Default test configurations
    std::vector<std::pair<std::string, std::vector<int>>> test_configs = {
        {"../hiveum_map_small.txt", {50, 100, 200}},
        {"../hiveum_map_medium.txt", {100, 500, 1000, 2000}}
    };
    
    // Override with command line arguments if provided
    if (argc >= 3) {
        test_configs.clear();
        std::string map_file = argv[2];
        std::vector<int> ant_counts;
        
        for (int i = 3; i < argc; i++) {
            ant_counts.push_back(std::stoi(argv[i]));
        }
        
        if (ant_counts.empty()) {
            ant_counts = {50, 100, 500, 1000}; // Default ant counts
        }
        
        test_configs.push_back({map_file, ant_counts});
    }
    
    BenchmarkRunner runner(executable);
    printHeader();
    
    // Run benchmarks
    for (const auto& config : test_configs) {
        const std::string& map_file = config.first;
        const std::vector<int>& ant_counts = config.second;
        
        for (int num_ants : ant_counts) {
            std::cout << "Running: " << map_file << " with " << num_ants << " ants..." << std::flush;
            
            BenchmarkResult result = runner.runBenchmark(map_file, num_ants);
            results.push_back(result);
            
            std::cout << " Done (" << std::fixed << std::setprecision(2) 
                      << result.execution_time_ms << " ms)" << std::endl;
            printResult(result);
        }
        std::cout << std::endl;
    }
    
    printSummary(results);
    
    return 0;
}
