cmake_minimum_required(VERSION 3.13)
project(TicTacToe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# First, add the globals library as it's needed by other components
add_subdirectory(Globals)

# Add the Board component
add_subdirectory(Board)

# Add the AI component
add_subdirectory(AI)

# Add the GUI component
add_subdirectory(GUI)

# Add the main console game executable
add_executable(tictactoe src/main.cpp)
target_link_libraries(tictactoe
    PRIVATE
        board
        ai
        globals
)
