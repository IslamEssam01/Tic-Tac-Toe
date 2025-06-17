cmake_minimum_required(VERSION 3.14)
project(TicTacToePerformance)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Performance test executable
add_executable(performance_test
    performance_test.cpp
    performance_monitor.cpp
)

target_include_directories(performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/components/auth/src
    ${CMAKE_SOURCE_DIR}/components/game/Board/src
    ${CMAKE_SOURCE_DIR}/components/game/AI/src
    ${CMAKE_SOURCE_DIR}/components/game/Globals/src
    ${CMAKE_SOURCE_DIR}/components/game_history/src
    ${CMAKE_SOURCE_DIR}/components/gui/src
)

target_link_libraries(performance_test
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        SQLite::SQLite3
        OpenSSL::Crypto
        auth
        board
        ai
        globals
        game_history
        gui
)

# GUI Performance test executable
add_executable(gui_performance_test
    gui_performance_test.cpp
    performance_monitor.cpp
)

target_include_directories(gui_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/components/auth/src
    ${CMAKE_SOURCE_DIR}/components/game/Board/src
    ${CMAKE_SOURCE_DIR}/components/game/AI/src
    ${CMAKE_SOURCE_DIR}/components/game/Globals/src
    ${CMAKE_SOURCE_DIR}/components/game_history/src
    ${CMAKE_SOURCE_DIR}/components/gui/src
)

target_link_libraries(gui_performance_test
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        SQLite::SQLite3
        OpenSSL::Crypto
        auth
        board
        ai
        globals
        game_history
        gui
)

# Add custom target to run performance tests
add_custom_target(performance_report
    COMMAND performance_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running performance tests and generating report"
    DEPENDS performance_test
)

# Performance report HTML generator
add_executable(generate_report
    generate_report.cpp
)

target_link_libraries(generate_report
    PRIVATE
        Qt6::Core
)
