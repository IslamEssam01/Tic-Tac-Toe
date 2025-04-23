cmake_minimum_required(VERSION 3.13)

# Create the GUI library
add_library(gui
    src/GameWindow.cpp
)

target_include_directories(gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link Qt libraries and other components
target_link_libraries(gui
    PUBLIC
        Qt6::Widgets
        board
        ai
        globals
)

# Add GUI executable
add_executable(tictactoe_gui src/main.cpp)
target_link_libraries(tictactoe_gui
    PRIVATE
        gui
)
