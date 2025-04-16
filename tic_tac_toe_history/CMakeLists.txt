cmake_minimum_required(VERSION 3.14) # FetchContent requires 3.14+
project(TicTacToeHistory)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite3
find_library(SQLITE3_LIBRARY NAMES sqlite3)
if (NOT SQLITE3_LIBRARY)
    message(FATAL_ERROR "SQLite3 not found. Please install libsqlite3-dev.")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/game_history.cpp
    src/database.cpp
)

# Main executable
add_executable(tic_tac_toe ${SOURCES})
target_link_libraries(tic_tac_toe ${SQLITE3_LIBRARY} pthread dl)

# Enable testing
enable_testing()

# Fetch GoogleTest from GitHub
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main # Use 'main' branch for latest stable version
)
# Prevent GoogleTest from overriding our compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
add_subdirectory(tests)