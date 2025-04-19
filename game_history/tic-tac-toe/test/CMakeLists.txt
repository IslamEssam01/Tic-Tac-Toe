# Find GTest package
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add test source files
set(TEST_SOURCES
    test_game.cpp
    test_database.cpp
    test_ai.cpp
)

# Create test executable
add_executable(run_tests ${TEST_SOURCES})

# Link test dependencies
target_link_libraries(run_tests
    GTest::GTest
    GTest::Main
    ${SQLite3_LIBRARIES}
)

# Make test source files have access to the main source files
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Add source files to test executable (excluding main.cpp)
target_sources(run_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/game.cpp
    ${CMAKE_SOURCE_DIR}/src/player.cpp
    ${CMAKE_SOURCE_DIR}/src/database.cpp
    ${CMAKE_SOURCE_DIR}/src/ai_player.cpp
)

# Add test to CTest
add_test(NAME AllTests COMMAND run_tests)