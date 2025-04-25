cmake_minimum_required(VERSION 3.10)
project(TicTacToeHistory)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SQLite3 REQUIRED)

# Enable automatic processing of Qt resources - THIS MUST COME BEFORE TARGETS
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages with specific version if needed
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(CORE_SOURCES
    src/game_history.cpp
)

# Core library
add_library(tictactoe_core ${CORE_SOURCES})
target_link_libraries(tictactoe_core PUBLIC SQLite::SQLite3)

# CLI executable
add_executable(tictactoe_history src/main.cpp)
target_link_libraries(tictactoe_history PRIVATE tictactoe_core)

# GUI executable 
add_executable(tictactoe_history_gui 
    src/game_history_gui.cpp
    src/gui_main.cpp
)

# Link Qt libraries (whether Qt5 or Qt6 was found)
if(Qt6_FOUND)
    target_link_libraries(tictactoe_history_gui PRIVATE 
        tictactoe_core 
        Qt6::Core 
        Qt6::Widgets
    )
else()
    target_link_libraries(tictactoe_history_gui PRIVATE 
        tictactoe_core 
        Qt5::Core 
        Qt5::Widgets
    )
endif()

# Enable testing
enable_testing()
add_subdirectory(test)