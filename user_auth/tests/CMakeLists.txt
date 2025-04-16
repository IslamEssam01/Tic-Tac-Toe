cmake_minimum_required(VERSION 3.10)

# Enable testing

enable_testing()

# Find GoogleTest package

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Define test sources

set(TEST_SOURCES test_user_auth.cpp test_database.cpp ${CMAKE_SOURCE_DIR}/src/auth/user_auth.cpp ${CMAKE_SOURCE_DIR}/src/db/database.cpp )

# Create test executable

add_executable(user_auth_tests ${TEST_SOURCES})

# Link libraries

target_link_libraries(user_auth_tests GTest::GTest GTest::Main SQLite::SQLite3 OpenSSL::Crypto )

# Add test to CTest

add_test(NAME UserAuthTests COMMAND user_auth_tests)