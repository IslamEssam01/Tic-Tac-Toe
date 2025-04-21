cmake_minimum_required(VERSION 3.10)

set(CMAKE_AUTOMOC ON)

# Find required packages for tests
find_package(Qt6 COMPONENTS Test REQUIRED)

# Create test executables
set(TEST_SOURCES
    test_login_page.cpp
    test_welcome_page.cpp
    test_main_window.cpp
    main_test.cpp
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Create a list of source files excluding main.cpp
set(APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main_window.cpp
    ${CMAKE_SOURCE_DIR}/src/login_page.cpp
    ${CMAKE_SOURCE_DIR}/src/welcome_page.cpp
    ${CMAKE_SOURCE_DIR}/src/auth/user_auth.cpp
    ${CMAKE_SOURCE_DIR}/src/db/database.cpp
)

add_executable(gui_tests ${TEST_SOURCES} ${APP_SOURCES})
target_link_libraries(gui_tests
    Qt6::Test
    Qt6::Core
    Qt6::Widgets
    SQLite::SQLite3
    OpenSSL::Crypto
)