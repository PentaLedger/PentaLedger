cmake_minimum_required(VERSION 3.15)

# Find or download Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Test sources
set(TEST_SOURCES
    test_data_file.cpp
)

# Create test executable
add_executable(pentaledger_tests ${TEST_SOURCES})

target_link_libraries(pentaledger_tests
    PRIVATE
    pentaledger
    GTest::gtest
    GTest::gtest_main
)

# Register tests
include(GoogleTest)
gtest_discover_tests(pentaledger_tests)

