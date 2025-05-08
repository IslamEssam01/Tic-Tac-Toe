cmake_minimum_required(VERSION 3.10)
project(TicTacToeHistory)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite package
find_package(SQLite3 REQUIRED)

# Find Qt package
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automatic handling of Qt resources, UI files, etc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Source files
set(SOURCES
    src/game_history.cpp
    src/game_history_gui.cpp
    src/gui_main.cpp
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable
add_executable(tictactoe_history ${SOURCES})
target_link_libraries(tictactoe_history 
    SQLite::SQLite3
    Qt6::Core 
    Qt6::Widgets
)

# Command line executable
add_executable(tictactoe_cli src/main.cpp src/game_history.cpp)
target_link_libraries(tictactoe_cli SQLite::SQLite3)

# Enable testing
enable_testing()
add_subdirectory(test)