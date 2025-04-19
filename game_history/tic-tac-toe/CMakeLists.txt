cmake_minimum_required(VERSION 3.10)
project(TicTacToeHistory VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite package
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Add source files
set(SOURCES
    src/main.cpp
    src/game.cpp
    src/player.cpp
    src/database.cpp
    src/ai_player.cpp
)

# Add header files
set(HEADERS
    src/game.h
    src/player.h
    src/database.h
    src/ai_player.h
)

# Create executable
add_executable(tictactoe ${SOURCES} ${HEADERS})

# Link SQLite
target_link_libraries(tictactoe ${SQLite3_LIBRARIES})

# Enable testing
enable_testing()
add_subdirectory(test)