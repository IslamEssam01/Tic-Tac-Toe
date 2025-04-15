cmake_minimum_required(VERSION 3.13)
project(TicTacToe)

set(CMAKE_CXX_STANDARD 17)

# Add the core library
add_library(board src/Board.cpp)
target_include_directories(board PUBLIC src)

# Add the main game executable
add_executable(tictactoe src/main.cpp)
target_link_libraries(tictactoe PRIVATE board)

# Proper GoogleTest Integration
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0 # latest stable release at the time of writing code
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add test executable
add_executable(test_board tests/test_board.cpp)
target_link_libraries(test_board
    PRIVATE
    board
    GTest::gtest_main
)

# Register the test
include(GoogleTest)
gtest_discover_tests(test_board)
