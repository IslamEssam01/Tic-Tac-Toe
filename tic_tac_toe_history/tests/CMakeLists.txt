cmake_minimum_required(VERSION 3.10)

# Test sources
set(TEST_SOURCES
    test_game_history.cpp
    test_database.cpp
    ${CMAKE_SOURCE_DIR}/src/game_history.cpp
    ${CMAKE_SOURCE_DIR}/src/database.cpp
)

# Test executable
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests gtest gtest_main ${SQLITE3_LIBRARY} pthread dl)

# Add test
add_test(NAME AllTests COMMAND run_tests)