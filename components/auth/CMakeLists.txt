cmake_minimum_required(VERSION 3.13)
project(AuthComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
find_package(OpenSSL REQUIRED)

# Create the auth library
add_library(auth
    src/auth/user_auth.cpp
    src/db/database.cpp
)

target_include_directories(auth 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(auth
    PUBLIC
        ${SQLITE3_LIBRARIES}
        OpenSSL::Crypto
)

target_include_directories(auth
    PUBLIC
        ${SQLITE3_INCLUDE_DIRS}
)

# Add tests if building tests
if(BUILD_TESTING)
    # Add auth tests
    add_executable(test_auth tests/test_user_auth.cpp src/auth/user_auth.cpp src/db/database.cpp)
    target_include_directories(test_auth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(test_auth
        PRIVATE
            GTest::gtest_main
            ${SQLITE3_LIBRARIES}
            OpenSSL::Crypto
    )

    add_executable(test_database tests/test_database.cpp src/db/database.cpp)
    target_include_directories(test_database PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(test_database
        PRIVATE
            GTest::gtest_main
            ${SQLITE3_LIBRARIES}
    )

    include(GoogleTest)
    gtest_discover_tests(test_auth)
    gtest_discover_tests(test_database)
endif()