cmake_minimum_required(VERSION 3.10) 
project(UserAuth)

set(CMAKE_CXX_STANDARD 17)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages

find_package(SQLite3 REQUIRED) 
find_package(OpenSSL REQUIRED)

# Include directories

include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files for main executable

set(SOURCES src/main.cpp src/auth/user_auth.cpp src/db/database.cpp )

# Main executable

add_executable(user_auth ${SOURCES})
target_link_libraries(user_auth SQLite::SQLite3 OpenSSL::Crypto)

# Add tests subdirectory

add_subdirectory(tests)