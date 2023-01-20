cmake_minimum_required(VERSION 3.25.1)

project(cabeza)

set (CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}\\bin)

add_executable(cabeza src/main.cpp src/graphics.cpp src/positioning.cpp src/performance.cpp src/graphics_console.cpp)