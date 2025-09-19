#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "ant_mania.h"

class AntManiaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a basic test map with all directions
        std::ofstream file("test_map.txt");
        file << "Center north=North south=South east=East west=West\n";
        file << "North south=Center\n";
        file << "South north=Center\n";
        file << "East west=Center\n";
        file << "West east=Center\n";
        file.close();
    }
    
    void TearDown() override {
        // Clean up test files
        std::remove("test_map.txt");
        std::remove("empty_map.txt");
    }
    
    void captureOutput() {
        old_cout = std::cout.rdbuf();
        std::cout.rdbuf(oss.rdbuf());
    }
    
    void restoreOutput() {
        std::cout.rdbuf(old_cout);
    }
    
private:
    std::ostringstream oss;
    std::streambuf* old_cout;
};

// Test 1: Basic map loading and direction parsing
TEST_F(AntManiaTest, MapLoadingAndDirectionParsing) {
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("test_map.txt"));
}

// Test 2: Ant creation and basic functionality
TEST_F(AntManiaTest, AntCreationAndBasicFunctionality) {
    AntManiaSimulation sim;
    sim.loadMap("test_map.txt");
    
    // Test creating ants
    EXPECT_NO_THROW(sim.createAnts(10));
    EXPECT_NO_THROW(sim.createAnts(0));  // Edge case: no ants
}

// Test 3: Complete simulation execution
TEST_F(AntManiaTest, CompleteSimulationExecution) {
    AntManiaSimulation sim;
    sim.loadMap("test_map.txt");
    sim.createAnts(5);  // Small number for quick test
    
    captureOutput();
    EXPECT_NO_THROW(sim.runSimulation());
    restoreOutput();
}

// Test 4: Error handling and edge cases
TEST_F(AntManiaTest, ErrorHandlingAndEdgeCases) {
    AntManiaSimulation sim;
    
    // Test with non-existent file
    EXPECT_FALSE(sim.loadMap("non_existent_file.txt"));
    
    // Test with empty file
    std::ofstream empty_file("empty_map.txt");
    empty_file.close();
    EXPECT_TRUE(sim.loadMap("empty_map.txt"));  // Should handle gracefully
}

// Test 5: Performance with larger scale
TEST_F(AntManiaTest, PerformanceWithLargerScale) {
    AntManiaSimulation sim;
    sim.loadMap("test_map.txt");
    
    // Test with larger number of ants
    EXPECT_NO_THROW(sim.createAnts(100));
    
    captureOutput();
    EXPECT_NO_THROW(sim.runSimulation());
    restoreOutput();
}