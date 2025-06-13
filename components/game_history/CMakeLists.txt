cmake_minimum_required(VERSION 3.14)
project(GameHistoryComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Find required packages
find_package(SQLite3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

# Enable Qt MOC for this component
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/src")

# Create the game_history library
add_library(game_history
    src/game_history.cpp
)

target_include_directories(game_history
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(game_history
    PUBLIC
        SQLite::SQLite3
        Qt6::Core
)

# Add tests if building tests
if(BUILD_TESTING)
    add_executable(test_game_history
        tests/test_game_history.cpp
        src/game_history.cpp
    )

    target_include_directories(test_game_history PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(test_game_history
        PRIVATE
            GTest::gtest_main
            SQLite::SQLite3
            Qt6::Core
    )

    include(GoogleTest)
    gtest_discover_tests(test_game_history)
endif()
