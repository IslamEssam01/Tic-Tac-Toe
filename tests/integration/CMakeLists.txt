cmake_minimum_required(VERSION 3.14)
project(IntegrationTests)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Ensure testing is enabled
if(NOT BUILD_TESTING)
    message(WARNING "BUILD_TESTING is not enabled. Integration tests will not be built.")
    return()
endif()

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

# Enable Qt MOC for integration tests
set(CMAKE_AUTOMOC ON)

# The component libraries are already built by the main CMakeLists.txt
# We just need to link against them, not rebuild them

# Game Workflow Integration Tests
add_executable(test_game_workflows
    test_game_workflows.cpp
)

target_include_directories(test_game_workflows PRIVATE
    ${CMAKE_SOURCE_DIR}/components/auth/src
    ${CMAKE_SOURCE_DIR}/components/game/Board/src
    ${CMAKE_SOURCE_DIR}/components/game/AI/src
    ${CMAKE_SOURCE_DIR}/components/game/Globals/src
    ${CMAKE_SOURCE_DIR}/components/game_history/src
    ${CMAKE_SOURCE_DIR}/components/gui/src
)

target_link_libraries(test_game_workflows
    PRIVATE
        GTest::gtest_main
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        auth
        board
        ai
        globals
        game_history
        gui
)

# GUI Integration Tests
add_executable(test_gui_integration
    test_gui_integration.cpp
)

target_include_directories(test_gui_integration PRIVATE
    ${CMAKE_SOURCE_DIR}/components/auth/src
    ${CMAKE_SOURCE_DIR}/components/game/Board/src
    ${CMAKE_SOURCE_DIR}/components/game/AI/src
    ${CMAKE_SOURCE_DIR}/components/game/Globals/src
    ${CMAKE_SOURCE_DIR}/components/game_history/src
    ${CMAKE_SOURCE_DIR}/components/gui/src
)

target_link_libraries(test_gui_integration
    PRIVATE
        GTest::gtest_main
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        auth
        board
        ai
        globals
        game_history
        gui
)

# Database Integration Tests
add_executable(test_database_integration
    test_database_integration.cpp
)

target_include_directories(test_database_integration PRIVATE
    ${CMAKE_SOURCE_DIR}/components/auth/src
    ${CMAKE_SOURCE_DIR}/components/game_history/src
)

target_link_libraries(test_database_integration
    PRIVATE
        GTest::gtest_main
        Qt6::Core
        auth
        game_history
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(test_game_workflows)
gtest_discover_tests(test_gui_integration)
gtest_discover_tests(test_database_integration)

# Add custom test target for integration tests only
add_custom_target(integration_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "test_.*_integration|test_game_workflows"
    COMMENT "Running integration tests"
)
