cmake_minimum_required(VERSION 3.25.1)

project(cabeza)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
 
set(CMAKE_EXE_LINKER_FLAGS "--static")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}\\bin)


add_executable(cabeza)

target_sources(cabeza
    PRIVATE 
        src/main.cpp
        src/graphics.cpp
        src/positioning.cpp
        src/eval.cpp
        src/graphics_console.cpp
        src/performance.cpp
        src/game.cpp
        src/search.cpp
        src/transposition.cpp
)


add_executable(console_graphics_helper)

target_sources(console_graphics_helper
    PRIVATE 
        src/console_graphics_helper.cpp
)

add_dependencies(cabeza console_graphics_helper)