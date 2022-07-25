#include <iostream>
#include "cpu.h"
#include "platform.h"
#include <chrono>
#include <thread>
#include <algorithm>

uint16_t video_height = 32;
uint16_t video_width = 64;
uint8_t  scale = 20;

int main(int argc, char* argv[]) {
    std::cout << "starting with " << argc << " arguments\n";
    CPU cpu;
    Platform platform("CHIP8 Emulator", video_width*scale, video_height*scale, video_width, video_height);
    cpu.init();
    if (!cpu.load_game("IBM Logo.ch8")) {
        std::cout << "no game found!" << '\n';
        return 1;
    }

    auto last_cycle = std::chrono::high_resolution_clock::now();
    uint16_t pitch = sizeof(cpu.gfx[0]) * video_width;
    uint8_t cycle_delay = 4;

    for (;;) {
        platform.handle_input(cpu.keypad);
        auto current_time = std::chrono::high_resolution_clock::now();
        float since_last = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle).count();
        if (since_last > cycle_delay) {
            last_cycle = current_time;
            cpu.single_cycle();

            if (cpu.draw_flag) {
                std::cout << "drawing screen" << std::endl;
                cpu.draw_flag = false;
            }

            platform.update(cpu.gfx, pitch);

            if (cpu.pause_execution) {
                system("pause");
            }
        }
    }

    return 0;
}
