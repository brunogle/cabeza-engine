cmake_minimum_required(VERSION 3.25.1)

set(MINGW32_DIRECTORY "C:\\Portable\\mingw64") #For windows compilation

project(cabeza)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
 
#set(CMAKE_EXE_LINKER_FLAGS "-pg")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}\\bin)



IF (WIN32)

include_directories("${MINGW32_DIRECTORY}\\opt\\include")
link_directories("${MINGW32_DIRECTORY}\\opt\\lib")
configure_file("${MINGW32_DIRECTORY}\\opt\\bin\\libreadline8.dll" ${CMAKE_CURRENT_BINARY_DIR}\\bin\\libreadline8.dll COPYONLY)
configure_file("${MINGW32_DIRECTORY}\\bin\\libgcc_s_seh-1.dll" ${CMAKE_CURRENT_BINARY_DIR}\\bin\\libgcc_s_seh-1.dll COPYONLY)
configure_file("${MINGW32_DIRECTORY}\\bin\\libstdc++-6.dll" ${CMAKE_CURRENT_BINARY_DIR}\\bin\\libstdc++-6.dll COPYONLY)

ENDIF()

add_executable(cabeza)

target_link_libraries(cabeza readline)

target_sources(cabeza
    PRIVATE 
        src/main.cpp
        src/positioning.cpp
        src/eval.cpp
        src/game.cpp
        src/search.cpp
        src/transposition.cpp
        src/parsing.cpp
        src/cui.cpp
)