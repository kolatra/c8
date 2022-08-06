#include <iostream>
#include "cpu.hh"
#include "display.hh"
#include <chrono>
#include <thread>
#include <algorithm>

bool command_line(char** start, char** end, std::string const& argument) {
    return std::find(start, end, argument) != end;
}

int main(int argc, char* argv[]) {
    CPU cpu;
    Display display(
            "CHIP8 Emulator",
            cpu.m_video_width * cpu.m_scale,
            cpu.m_video_height * cpu.m_scale,
            cpu.m_video_width,
            cpu.m_video_height);
    cpu.init();
    std::vector<std::string> roms = { "IBM Logo.ch8", "PONG.ch8" };
    if (!cpu.load_game(roms[1])) {
        printf("no game found!\n");
        exit(1);
    }

    if (command_line(argv, argv+argc, "-p")) {
        cpu.pause_execution = true;
    }

    auto last_cycle = std::chrono::high_resolution_clock::now();
    uint16_t pitch = sizeof(cpu.m_gfx[0]) * cpu.m_video_width;
    uint8_t cycle_delay = 4;

    for (;;) {
        display.handle_input(cpu.keypad);
        auto current_time = std::chrono::high_resolution_clock::now();
        float since_last = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle).count();
        if (since_last > cycle_delay) {
            last_cycle = current_time;
            cpu.single_cycle();
            display.update(cpu.m_gfx, pitch, cpu);
        }
    }
}
