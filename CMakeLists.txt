cmake_minimum_required(VERSION 3.25.1)

project(cabeza)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}\\bin)

add_executable(cabeza src/main.cpp src/graphics.cpp src/positioning.cpp src/eval.cpp src/graphics_console.cpp src/performance.cpp)

add_executable(console_graphics_helper src/console_graphics_helper.cpp)