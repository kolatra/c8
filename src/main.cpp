#include <iostream>
#include "cpu.h"
#include <chrono>
#include <thread>

CPU cpu;
bool running = true;

int main(int argc, char **argv) {
    std::cout << "Starting with " << argc << " arguments\n";
    cpu.init();
    if (!cpu.load_game("PONG.ch8")) {
        return 1;
    }

    while (running) {
        cpu.lie_to_the_people();

        if (cpu.draw_flag) {
            std::cout << "drawing screen" << std::endl;
            cpu.draw_flag = false;
        }

        cpu.set_keys();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 5));
    }
    return 0;
}
