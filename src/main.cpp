#include <iostream>
#include "cpu.h"
#include "display.h"
#include <chrono>
#include <thread>
#include <algorithm>

int main(int argc, char* argv[]) {
    std::cout << "starting with " << argc << " arguments\n";
    CPU cpu;
    Display platform(
            "CHIP8 Emulator",
            cpu.video_width*cpu.scale,
            cpu.video_height*cpu.scale,
            cpu.video_width,
            cpu.video_height);
    cpu.init();
    if (!cpu.load_game("IBM Logo.ch8")) {
        std::cout << "no game found!" << '\n';
        exit(1);
    }

    auto last_cycle = std::chrono::high_resolution_clock::now();
    uint16_t pitch = sizeof(cpu.gfx[0]) * cpu.video_width;
    uint8_t cycle_delay = 4;

    for (;;) {
        platform.handle_input(cpu.keypad);
        auto current_time = std::chrono::high_resolution_clock::now();
        float since_last = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle).count();
        if (since_last > cycle_delay) {
            last_cycle = current_time;
            cpu.single_cycle();
            if (cpu.draw_flag) {
                platform.update(cpu.gfx, pitch, cpu);
                cpu.draw_flag = false;
            }
        }
    }
}
