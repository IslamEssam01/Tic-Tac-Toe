cmake_minimum_required(VERSION 3.10)
project(TicTacToeHistory)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite package
find_package(SQLite3 REQUIRED)

# Source files
set(SOURCES
    src/game_history.cpp
    src/main.cpp
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable
add_executable(tictactoe_history ${SOURCES})
target_link_libraries(tictactoe_history SQLite::SQLite3)

# Enable testing
enable_testing()
add_subdirectory(test)