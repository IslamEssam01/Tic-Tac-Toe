cmake_minimum_required(VERSION 3.13)
project(GUIComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Setup Qt for this component
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

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

# Add tests if building tests
if(BUILD_TESTING)
    find_package(Qt6 COMPONENTS Test REQUIRED)
    
    add_executable(test_gamewindow
        tests/test_GameWindow.cpp
    )
    
    target_include_directories(test_gamewindow PRIVATE
        src
    )
    
    target_link_libraries(test_gamewindow
        PRIVATE
            GTest::gtest_main
            Qt6::Core
            Qt6::Widgets
            Qt6::Test
            gui
    )
    
    include(GoogleTest)
    gtest_discover_tests(test_gamewindow)
endif()
