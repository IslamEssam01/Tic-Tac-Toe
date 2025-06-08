cmake_minimum_required(VERSION 3.13)
project(AppTests)

# Include common settings (GoogleTest already configured there)
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

find_package(Qt6 COMPONENTS Core Widgets Test REQUIRED)

# Create the test_login_page executable
add_executable(test_login_page
    test_login_page.cpp
    ../src/login_page.cpp
)

target_include_directories(test_login_page PRIVATE
    ../src
    ../../components/auth/src
    ../../components/game/Globals/src
)

target_link_libraries(test_login_page
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        auth
)

# Create the test_main_window executable
add_executable(test_main_window
    test_main_window.cpp
    ../src/main_window.cpp
    ../src/login_page.cpp
)

target_include_directories(test_main_window PRIVATE
    ../src
    ../../components/auth/src
    ../../components/game/Globals/src
    ../../components/game/Board/src
    ../../components/game/AI/src
    ../../components/game/GUI/src
)

target_link_libraries(test_main_window
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        auth
        game
)

include(GoogleTest)
gtest_discover_tests(test_login_page)
gtest_discover_tests(test_main_window)
