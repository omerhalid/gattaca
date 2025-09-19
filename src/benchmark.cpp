#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <executable_path> [map_file] [ant_counts...]" << std::endl;
        std::cout << "Example: " << argv[0] << " ./ant_mania ../hiveum_map_small.txt 50 100 500" << std::endl;
        return 1;
    }
    
    std::string executable = argv[1];
    std::vector<std::string> test_configs;
    
    // Default tests if no arguments provided
    if (argc == 2) {
        test_configs = {
            "../hiveum_map_small.txt 50",
            "../hiveum_map_small.txt 100", 
            "../hiveum_map_medium.txt 1000",
            "../hiveum_map_medium.txt 2000"
        };
    } else {
        // Use provided arguments
        std::string map_file = argv[2];
        for (int i = 3; i < argc; i++) {
            test_configs.push_back(map_file + " " + argv[i]);
        }
    }
    
    std::cout << "=== Ant Mania Benchmark ===" << std::endl;
    std::cout << std::left << std::setw(25) << "Map & Ants" << std::setw(15) << "Time (ms)" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    double total_time = 0;
    
    for (const auto& config : test_configs) {
        std::string cmd = executable + " " + config + " > /dev/null 2>&1";
        
        auto start = std::chrono::high_resolution_clock::now();
        int result = system(cmd.c_str());
        auto end = std::chrono::high_resolution_clock::now();
        
        if (result == 0) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            double time_ms = duration.count() / 1000.0;
            total_time += time_ms;
            
            // Parse config to show cleaner output
            size_t space_pos = config.find(' ');
            std::string map_part = config.substr(0, space_pos);
            std::string ant_count = config.substr(space_pos + 1);
            
            // Extract just the map name (remove path)
            size_t slash_pos = map_part.find_last_of('/');
            std::string map_name = (slash_pos != std::string::npos) ? map_part.substr(slash_pos + 1) : map_part;
            
            std::string display_name = map_name + " (" + ant_count + " ants)";
            
            std::cout << std::left << std::setw(25) << display_name 
                      << std::setw(15) << std::fixed << std::setprecision(2) << time_ms << std::endl;
        } else {
            std::cout << "Error running: " << config << std::endl;
        }
    }
    
    std::cout << std::string(40, '-') << std::endl;
    std::cout << "Average time: " << std::fixed << std::setprecision(2) 
              << (total_time / test_configs.size()) << " ms" << std::endl;
    
    return 0;
}
