cmake_minimum_required(VERSION 3.13)

# Add the globals library
add_library(globals globals.cpp)
target_include_directories(globals PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
