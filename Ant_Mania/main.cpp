#include "ant_mania.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <map_file> <num_ants>" << std::endl;
        std::cout << "Example: " << argv[0] << " hiveum_map_small.txt 100" << std::endl;
        return 1;
    }
    
    std::string map_file = argv[1];
    uint32_t num_ants = std::stoul(argv[2]);
    
    if (num_ants <= 0) {
        std::cerr << "Error: Number of ants must be positive" << std::endl;
        return 1;
    }
    
    AntManiaSimulation simulation;
    
    // Load map
    if (!simulation.loadMap(map_file)) {
        return 1;
    }
    
    // Create ants
    simulation.createAnts(num_ants);
    
    // Run simulation
    simulation.runSimulation();
    
    // Print results
    simulation.printStatistics();
    simulation.printRemainingWorld();
    
    return 0;
}
