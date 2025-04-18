cmake_minimum_required(VERSION 3.13)

# Add the board library
add_library(board src/Board.cpp)
target_include_directories(board 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(board PUBLIC globals)

# Add test executable
add_executable(test_board tests/test_board.cpp)
target_link_libraries(test_board
    PRIVATE
    board
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_board)

enable_testing()
