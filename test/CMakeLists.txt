cmake_minimum_required(VERSION 3.16)
project(AntManiaTests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable
add_executable(test_ant_mania 
    test_ant_mania.cpp
    ../src/ant_mania.cpp
)

# Compiler flags for tests (less aggressive than main build)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

# Include directories
target_include_directories(test_ant_mania PRIVATE ../include)

# Link Google Test
target_link_libraries(test_ant_mania GTest::GTest GTest::Main)

# Enable testing
enable_testing()
add_test(NAME AntManiaTests COMMAND test_ant_mania)

# Print test configuration
message(STATUS "Test build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Test C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Google Test found: ${GTEST_FOUND}")
