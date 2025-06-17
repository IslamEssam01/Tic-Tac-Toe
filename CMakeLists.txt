cmake_minimum_required(VERSION 3.14)
project(TicTacToe)

# Include common settings for all components
include(cmake/CommonSettings.cmake)

# Add the main application
add_subdirectory(app)

# Optionally build individual components
option(BUILD_COMPONENTS_STANDALONE "Build components as standalone projects" OFF)
if(BUILD_COMPONENTS_STANDALONE)
    add_subdirectory(components/auth)
    add_subdirectory(components/game)
    add_subdirectory(components/game_history)
endif()

# Add integration tests if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(tests/integration)
endif()

# Add performance tests
add_subdirectory(performance)
