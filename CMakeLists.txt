cmake_minimum_required(VERSION 3.7)
project(chip8)

set(CMAKE_CXX_STANDARD 14)
set(SDL2_INCLUDE_DIR C:/libs/SDL/include)
set(SDL2_LIB_DIR C:/libs/SDL/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(chip8 src/main.cc src/cpu.cc src/cpu.hh src/display.cc src/display.hh)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2)
