cmake_minimum_required(VERSION 3.13)

# Create the AI library
add_library(ai src/AI.cpp)
target_include_directories(ai 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link against board and globals
target_link_libraries(ai PUBLIC board globals)

# Add tests
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add AI test executable
add_executable(test_ai test/test_AI.cpp)
target_link_libraries(test_ai
    PRIVATE
    ai
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_ai)
