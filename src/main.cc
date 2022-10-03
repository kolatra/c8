#include <chrono>
#include <algorithm>
#include <Windows.h>
#include "cpu.hh"
#include "display.hh"
#include <filesystem>
#include <iostream>
#include <vector>

bool command_line(char** start, char** end, std::string const& argument) {
    return std::find(start, end, argument) != end;
}

int main(int argc, char* argv[]) {
    printf("starting with %d args\n", argc);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Error while starting" << SDL_GetError() << '\n';
        return -1;
    }
    CPU cpu;
    Display display("chip8");
    display.clear();
    cpu.init(display);

    std::string rom = "IBM Logo.ch8";
    //std::string rom = "PONG.ch8";
    //std::string rom = "test_opcode.ch8";
    constexpr int selected = 0;
    if (!cpu.load_game(rom)) {
        printf("no game found!\n");
        exit(1);
    }
    printf("Loading %s\n", rom.c_str());

    if (command_line(argv, argv + argc, "-p")) {
        cpu.m_pause_execution = true;
    }

    auto last_cycle = std::chrono::high_resolution_clock::now();
    uint8_t cycle_delay = 4;

    for (;;) {
        display.handle_input(cpu.m_keypad);
        auto current_time = std::chrono::high_resolution_clock::now();
        float since_last = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle).count();
        if (since_last > cycle_delay) {
            last_cycle = current_time;
            cpu.single_cycle();
        }
    }
}
