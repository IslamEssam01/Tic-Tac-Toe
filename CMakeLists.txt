cmake_minimum_required(VERSION 3.10)
project(TicTacToe)

set(CMAKE_CXX_STANDARD 17)

# ملفات الكود الأساسي
add_library(board src/Board.cpp)
target_include_directories(board PUBLIC src)

# إضافة مكتبة GoogleTest
add_subdirectory(googletest)
include_directories(googletest/googletest/include)

# ملف الاختبارات
enable_testing()

add_executable(test_board tests/test_board.cpp)
target_link_libraries(test_board board gtest gtest_main)

add_test(NAME BoardTest COMMAND test_board)

