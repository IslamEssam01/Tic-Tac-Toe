cmake_minimum_required(VERSION 3.12)
project(TicTacToeHistory)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Main executable
add_executable(tictactoe_history main.cpp src/game_history.cpp)
target_include_directories(tictactoe_history PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tictactoe_history SQLite::SQLite3)

# Include test directory
add_subdirectory(test)