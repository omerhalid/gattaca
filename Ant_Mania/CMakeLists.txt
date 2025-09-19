cmake_minimum_required(VERSION 3.16)
project(AntManiaOptimized)

# Set C++ standard to C++17 for modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimized compiler flags for maximum performance
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native -flto -ffast-math -funroll-loops -finline-functions -fomit-frame-pointer")

# Enable debug information for debug builds
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Create executable
add_executable(ant_mania 
    ant_mania.cpp
    main.cpp
)

# Create benchmark tool
add_executable(benchmark 
    benchmark.cpp
)

# Set default build type to Release for performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

# Compiler-specific optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ant_mania PRIVATE -Wall -Wextra)
    target_compile_options(benchmark PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(ant_mania PRIVATE -Wall -Wextra)
    target_compile_options(benchmark PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(ant_mania PRIVATE /W4 /O2 /Ob2 /Oi /Ot /Oy /GL)
    target_compile_options(benchmark PRIVATE /W4 /O2 /Ob2 /Oi /Ot /Oy /GL)
endif()
