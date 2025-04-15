cmake_minimum_required(VERSION 3.13)
project(TicTacToe)

set(CMAKE_CXX_STANDARD 17)

# Add the globals library first
add_library(globals ../Globals/globals.cpp)
target_include_directories(globals PUBLIC ../Globals)

# Add the core library
add_library(board src/Board.cpp)
target_include_directories(board PUBLIC src)
target_link_libraries(board PUBLIC globals)  # Link board with globals

# Add the main game executable
add_executable(tictactoe src/main.cpp)
target_link_libraries(tictactoe PRIVATE board)

# Proper GoogleTest Integration
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add test executable
add_executable(test_board tests/test_board.cpp)
target_link_libraries(test_board
    PRIVATE
    board
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_board)
