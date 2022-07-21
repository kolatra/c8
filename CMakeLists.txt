cmake_minimum_required(VERSION 3.7)
project(untitled)

set(CMAKE_CXX_STANDARD 14)
set(SDL2_INCLUDE_DIR C:/libs/SDL/include)
set(SDL2_LIB_DIR C:/libs/SDL/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(untitled src/main.cpp src/cpu.cpp src/cpu.h src/Platform.cpp src/Platform.h)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2)
