#include <iostream>
#include "cpu.h"

CPU cpu;

int main(int argc, char **argv) {
    std::cout << "Starting with " << argc << " arguments\n";
    cpu.init();
    cpu.load_game("PONG");

    for (;;) {
        cpu.lie_to_the_people();

        if (cpu.drawFlag)
            std::cout << "drawing screen" << std::endl;

        cpu.set_keys();
    }
    return 0;
}
