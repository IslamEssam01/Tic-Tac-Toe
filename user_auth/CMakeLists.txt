cmake_minimum_required(VERSION 3.10)
project(UserAuthQt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Source files for main executable
set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/login_page.cpp
    src/welcome_page.cpp
    src/auth/user_auth.cpp
    src/db/database.cpp
)

# Main executable
add_executable(user_auth_qt ${SOURCES})
target_link_libraries(user_auth_qt
    Qt6::Core
    Qt6::Widgets
    SQLite::SQLite3
    OpenSSL::Crypto
)

# Add tests subdirectory
add_subdirectory(tests)