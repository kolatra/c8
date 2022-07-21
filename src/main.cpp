#include <iostream>
#include "cpu.h"
#include <chrono>
#include <thread>
#include <algorithm>

CPU cpu;

int main(int argc, char* argv[]) {
    std::cout << "starting with " << argc << " arguments\n";
    cpu.init();
    if (!cpu.load_game("PONG.ch8")) {
        std::cout << "no game found!" << '\n';
        return 1;
    }

    auto lastCycle = std::chrono::high_resolution_clock::now();
    bool running = true;
    int cycleDelay = 4;

    while (running) {
        //running = check for escape key
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycle).count();
        if (dt > cycleDelay) {
            lastCycle = currentTime;
            cpu.single_cycle();
            cpu.set_keys();

            if (cpu.draw_flag) {
                std::cout << "drawing screen" << std::endl;
                cpu.draw_flag = false;
            }

            if (cpu.pause_execution) {
                system("pause");
            }
        }
    }

    return 0;
}
