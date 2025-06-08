cmake_minimum_required(VERSION 3.14)
project(GameComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)



# First, add the globals library as it's needed by other components
add_subdirectory(Globals)

# Add the Board component
add_subdirectory(Board)

# Add the AI component
add_subdirectory(AI)



# Create a combined game component library that includes all game modules
add_library(game INTERFACE)
target_link_libraries(game INTERFACE
    board
    ai
    globals
)

# Add the main console game executable
add_executable(tictactoe_console src/main.cpp)
target_link_libraries(tictactoe_console
    PRIVATE
        board
        ai
        globals
)
