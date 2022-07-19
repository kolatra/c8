#include <iostream>
#include "cpu.h"
#include <chrono>
#include <thread>
#include <algorithm>

CPU cpu;
bool running = true;

int main(int argc, char **argv) {
    std::cout << "starting with " << argc << " arguments\n";
    cpu.init();
    if (!cpu.load_game("PONG.ch8")) {
        std::cout << "no game found!" << '\n';
        return 1;
    }

    while (running) {
        cpu.single_cycle();

        if (cpu.draw_flag) {
            std::cout << "drawing screen" << std::endl;
            cpu.draw_flag = false;
        }

        cpu.set_keys();
        std::this_thread::sleep_for(std::chrono::seconds(1 / 50));

        if (cpu.pause_execution) {
            system("pause");
        }
    }
    return 0;
}
