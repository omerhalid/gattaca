#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "../ant_mania.h"

class AntManiaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple test map
        std::ofstream file("test_map.txt");
        file << "A north=B south=C\n";
        file << "B south=A\n";
        file << "C north=A\n";
        file.close();
        
        // Create a more complex test map
        std::ofstream complex_file("complex_map.txt");
        complex_file << "Start north=Middle east=End\n";
        complex_file << "Middle south=Start north=End\n";
        complex_file << "End west=Start south=Middle\n";
        complex_file.close();
    }
    
    void TearDown() override {
        // Clean up test files
        std::remove("test_map.txt");
        std::remove("complex_map.txt");
        std::remove("direction_test.txt");
        std::remove("empty_map.txt");
    }
};

// Test direction parsing by creating maps with all directions
TEST_F(AntManiaTest, DirectionParsing) {
    std::ofstream file("direction_test.txt");
    file << "A north=B south=C east=D west=E\n";
    file << "B south=A\n";
    file << "C north=A\n";
    file << "D west=A\n";
    file << "E east=A\n";
    file.close();
    
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("direction_test.txt"));
}

// Test basic map loading
TEST_F(AntManiaTest, MapLoading) {
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("test_map.txt"));
}

// Test ant creation with various counts
TEST_F(AntManiaTest, AntCreation) {
    AntManiaSimulation sim;
    sim.loadMap("test_map.txt");
    
    // Test creating different numbers of ants
    EXPECT_NO_THROW(sim.createAnts(5));
    EXPECT_NO_THROW(sim.createAnts(0));
    EXPECT_NO_THROW(sim.createAnts(100));
}

// Test simulation execution
TEST_F(AntManiaTest, SimulationExecution) {
    AntManiaSimulation sim;
    sim.loadMap("complex_map.txt");
    sim.createAnts(2);  // Small number for quick test
    
    // Capture output to avoid cluttering test output
    std::ostringstream oss;
    std::streambuf* old_cout = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    
    EXPECT_NO_THROW(sim.runSimulation());
    
    // Restore cout
    std::cout.rdbuf(old_cout);
}

// Test edge cases
TEST_F(AntManiaTest, EdgeCases) {
    AntManiaSimulation sim;
    
    // Test with non-existent file
    EXPECT_FALSE(sim.loadMap("non_existent_file.txt"));
    
    // Test with empty file
    std::ofstream empty_file("empty_map.txt");
    empty_file.close();
    
    EXPECT_TRUE(sim.loadMap("empty_map.txt"));  // Should handle empty file gracefully
}

// Test map with single colony
TEST_F(AntManiaTest, SingleColonyMap) {
    std::ofstream file("single_colony.txt");
    file << "LonelyColony\n";
    file.close();
    
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("single_colony.txt"));
    EXPECT_NO_THROW(sim.createAnts(1));
    
    std::remove("single_colony.txt");
}

// Test map with disconnected colonies
TEST_F(AntManiaTest, DisconnectedColonies) {
    std::ofstream file("disconnected.txt");
    file << "Island1\n";
    file << "Island2\n";
    file.close();
    
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("disconnected.txt"));
    EXPECT_NO_THROW(sim.createAnts(2));
    
    std::remove("disconnected.txt");
}

// Test large number of ants
TEST_F(AntManiaTest, LargeAntCount) {
    AntManiaSimulation sim;
    sim.loadMap("complex_map.txt");
    EXPECT_NO_THROW(sim.createAnts(1000));
}

// Test map with maximum connections (4 directions)
TEST_F(AntManiaTest, MaxConnections) {
    std::ofstream file("max_connections.txt");
    file << "Center north=North south=South east=East west=West\n";
    file << "North south=Center\n";
    file << "South north=Center\n";
    file << "East west=Center\n";
    file << "West east=Center\n";
    file.close();
    
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("max_connections.txt"));
    EXPECT_NO_THROW(sim.createAnts(10));
    
    std::remove("max_connections.txt");
}

// Test invalid map format
TEST_F(AntManiaTest, InvalidMapFormat) {
    std::ofstream file("invalid_map.txt");
    file << "Colony1 invalid_direction=Colony2\n";
    file << "Colony2\n";
    file.close();
    
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("invalid_map.txt"));  // Should handle gracefully
    
    std::remove("invalid_map.txt");
}

// Test simulation with no ants
TEST_F(AntManiaTest, NoAntsSimulation) {
    AntManiaSimulation sim;
    sim.loadMap("test_map.txt");
    sim.createAnts(0);
    
    // Capture output
    std::ostringstream oss;
    std::streambuf* old_cout = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    
    EXPECT_NO_THROW(sim.runSimulation());
    
    // Restore cout
    std::cout.rdbuf(old_cout);
}

// Test simulation with single ant
TEST_F(AntManiaTest, SingleAntSimulation) {
    AntManiaSimulation sim;
    sim.loadMap("test_map.txt");
    sim.createAnts(1);
    
    // Capture output
    std::ostringstream oss;
    std::streambuf* old_cout = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    
    EXPECT_NO_THROW(sim.runSimulation());
    
    // Restore cout
    std::cout.rdbuf(old_cout);
}