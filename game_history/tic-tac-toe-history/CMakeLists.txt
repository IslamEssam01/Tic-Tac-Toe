cmake_minimum_required(VERSION 3.10)
project(TicTacToeHistory)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite package
find_package(SQLite3 REQUIRED)

# Find Qt package - using Qt6 if available, falling back to Qt5 if not
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
endif()

# Enable automatic handling of Qt resources, UI files, etc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(GUI_SOURCES
    src/game_history.cpp
    src/game_history_gui.cpp
    src/gui_main.cpp
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main GUI executable
add_executable(tictactoe_history ${GUI_SOURCES})
if (Qt6_FOUND)
    target_link_libraries(tictactoe_history 
        SQLite::SQLite3
        Qt6::Core 
        Qt6::Widgets
    )
else()
    target_link_libraries(tictactoe_history 
        SQLite::SQLite3
        Qt5::Core 
        Qt5::Widgets
    )
endif()

# Command line executable (no Qt dependency)
add_executable(tictactoe_cli src/main.cpp src/game_history.cpp)
target_link_libraries(tictactoe_cli SQLite::SQLite3)

# Enable testing
enable_testing()
add_subdirectory(test)