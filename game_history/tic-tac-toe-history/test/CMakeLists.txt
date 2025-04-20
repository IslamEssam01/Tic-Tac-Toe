# Find GoogleTest
find_package(GTest REQUIRED)

# Add test executable
add_executable(game_history_test test_game_history.cpp ../src/game_history.cpp)

# Include directories
target_include_directories(game_history_test PRIVATE ${CMAKE_SOURCE_DIR})

# Link libraries
target_link_libraries(game_history_test
    GTest::GTest
    GTest::Main
    SQLite::SQLite3
)

# Add to CTest system
include(GoogleTest)
gtest_discover_tests(game_history_test)