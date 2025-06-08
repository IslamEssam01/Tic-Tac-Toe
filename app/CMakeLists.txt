cmake_minimum_required(VERSION 3.14)
project(TicTacToeApp)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Add component subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/components/auth ${CMAKE_BINARY_DIR}/auth_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/components/game ${CMAKE_BINARY_DIR}/game_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/components/gui ${CMAKE_BINARY_DIR}/gui_build)

# Create the main application executable
add_executable(tictactoe_app
    src/main.cpp
)

target_include_directories(tictactoe_app PRIVATE
    ${CMAKE_SOURCE_DIR}/components/gui/src
)

target_link_libraries(tictactoe_app
    PRIVATE
        gui
)
