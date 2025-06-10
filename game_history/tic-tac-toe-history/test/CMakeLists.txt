# Find GoogleTest package
find_package(GTest REQUIRED)

# Test source files
set(TEST_SOURCES
    game_history_test.cpp
    ${CMAKE_SOURCE_DIR}/src/game_history.cpp
)

# Create test executable
add_executable(tictactoe_history_tests ${TEST_SOURCES})

# Include directories
target_include_directories(tictactoe_history_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(tictactoe_history_tests
    GTest::GTest
    GTest::Main
    SQLite::SQLite3
)

# Add tests to CTest
add_test(NAME tictactoe_history_tests COMMAND tictactoe_history_tests)