cmake_minimum_required(VERSION 3.13)
project(TicTacToeApp)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Setup Qt for this component
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Add component subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/components/auth ${CMAKE_BINARY_DIR}/auth_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/components/game ${CMAKE_BINARY_DIR}/game_build)

# Create the main application executable
add_executable(tictactoe_app
    src/main.cpp
    src/main_window.cpp
    src/login_page.cpp
)

target_include_directories(tictactoe_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/components/auth/src
    ${CMAKE_SOURCE_DIR}/components/game/Globals/src
    ${CMAKE_SOURCE_DIR}/components/game/Board/src
    ${CMAKE_SOURCE_DIR}/components/game/AI/src
    ${CMAKE_SOURCE_DIR}/components/game/GUI/src
)

target_link_libraries(tictactoe_app
    PRIVATE
        Qt6::Widgets
        auth
        game
)

# Add tests if building tests
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
