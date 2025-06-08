cmake_minimum_required(VERSION 3.14)
project(GlobalsComponent)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Create the globals library
add_library(globals src/globals.cpp)

target_include_directories(globals 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
