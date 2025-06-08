cmake_minimum_required(VERSION 3.14)
project(AIComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Create the AI library
add_library(ai src/AI.cpp)
target_include_directories(ai 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link against board and globals
target_link_libraries(ai PUBLIC board globals)

# Add tests if building tests
if(BUILD_TESTING)
    # Add AI test executable
    add_executable(test_ai tests/test_AI.cpp)
    target_link_libraries(test_ai
        PRIVATE
        ai
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(test_ai)
endif()
