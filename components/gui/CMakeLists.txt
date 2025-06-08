cmake_minimum_required(VERSION 3.13)
project(GUIComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Setup Qt for this component
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Create the GUI library
add_library(gui
    src/application.cpp
    src/main_window.cpp
    src/login_page.cpp
    src/game_window.cpp
)

target_include_directories(gui 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link Qt libraries and other components
target_link_libraries(gui
    PUBLIC
        Qt6::Widgets
        auth
        board
        ai
        globals
)

# Add tests if building tests
if(BUILD_TESTING)
    find_package(Qt6 COMPONENTS Test REQUIRED)
    
    add_executable(test_login_page
        tests/test_login_page.cpp
    )
    
    target_include_directories(test_login_page PRIVATE
        src
    )
    
    target_link_libraries(test_login_page
        PRIVATE
            GTest::gtest_main
            GTest::gmock_main
            Qt6::Core
            Qt6::Widgets
            Qt6::Test
            gui
    )
    
    add_executable(test_main_window
        tests/test_main_window.cpp
    )
    
    target_include_directories(test_main_window PRIVATE
        src
    )
    
    target_link_libraries(test_main_window
        PRIVATE
            GTest::gtest_main
            GTest::gmock_main
            Qt6::Core
            Qt6::Widgets
            Qt6::Test
            gui
    )
    
    add_executable(test_game_window
        tests/test_game_window.cpp
    )
    
    target_include_directories(test_game_window PRIVATE
        src
    )
    
    target_link_libraries(test_game_window
        PRIVATE
            GTest::gtest_main
            Qt6::Core
            Qt6::Widgets
            Qt6::Test
            gui
    )
    
    include(GoogleTest)
    gtest_discover_tests(test_login_page)
    gtest_discover_tests(test_main_window)
    gtest_discover_tests(test_game_window)
endif()