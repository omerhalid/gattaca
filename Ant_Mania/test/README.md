# Ant Mania Test Suite

Professional test framework using Google Test for the Ant Mania simulation.

## Prerequisites

- **Google Test**: Required for running tests
- **CMake 3.16+**: For building tests
- **C++17**: Compiler support

### Installing Google Test

**macOS (Homebrew):**
```bash
brew install googletest
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libgtest-dev
```

## Running Tests

```bash
# From the root directory (recommended)
./run_tests.sh

# Or manually from test directory
cd test
mkdir -p build
cd build
cmake ..
make test_ant_mania
./test_ant_mania

# Or using CTest
ctest --verbose
```

## Test Coverage

The test suite covers:

- **Direction parsing**: Tests all direction types (north, south, east, west)
- **Map loading**: Tests file parsing and colony creation
- **Ant creation**: Tests ant initialization with various counts
- **Simulation execution**: Tests basic simulation flow
- **Edge cases**: Tests error handling and boundary conditions
- **Single colony maps**: Tests maps with isolated colonies
- **Disconnected colonies**: Tests maps with no connections
- **Large ant counts**: Tests scalability
- **Invalid formats**: Tests graceful error handling

## Test Structure

- `test_ant_mania.cpp`: Main test file using Google Test framework
- `CMakeLists.txt`: Build configuration with Google Test integration
- `../run_tests.sh`: Test runner script in root directory

## Adding New Tests

To add new tests using Google Test:

1. Add a new test function: `TEST_F(AntManiaTest, TestName)`
2. Use Google Test assertions: `EXPECT_TRUE()`, `EXPECT_FALSE()`, `EXPECT_NO_THROW()`
3. Follow the existing pattern of creating temporary files in `SetUp()` and cleaning up in `TearDown()`

Example:
```cpp
TEST_F(AntManiaTest, NewFeatureTest) {
    AntManiaSimulation sim;
    EXPECT_TRUE(sim.loadMap("test_map.txt"));
    // Add your test logic here
}
```
