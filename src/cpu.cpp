#include <iostream>
#include <cstring>
#include "cpu.h"

const unsigned int FONTSET_START = 0x50;

void CPU::init() {
    pc     = 0x200;
    opcode = 0;
    index  = 0;
    sp     = 0;

    // Clear display
    // Clear stack
    // Clear registers V0-VF
    // Clear memory

    for (int i = 0; i < 80; ++i) {
        memory[FONTSET_START + i] = fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

void CPU::load_game(std::string const &name) {

}

void CPU::pretend_cycle() {
    /**
     * fetch opcode from the memory at the location stored in the program counter
     * data is stored in an array in which each address contains one byte
     *
     * in the assumption of
     * memory[pc]     == 0xA2
     * memory[pc + 1] == 0xF0
     *
     * 0xA2 was shifted left 8 bits, which adds 8 zeroes
     *
     * 0xA2       0xA2 << 8 = 0xA200   HEX
     * 10100010   1010001000000000     BIN
     */
    //opcode = memory[pc] << 8 | memory[pc + 1];
    opcode = 0xA2F0; // sets the address of the index register to the value of NNN, in this case 0x2F0
    std::cout << opcode << '\n';


    // parse opcode and execute
    switch (opcode & 0xF000) {
        case 0xA000:
            index = opcode & 0x0FFF;
            pc += 2; // maybe just do this under the switch? if the next opcode should be skipped, increment by 4
            break;
        case 0x00E0:
            clear_display_00e0();
            break;
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    if (delay_timer > 0)
        --delay_timer;
    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void CPU::set_keys() {

}

void CPU::clear_display_00e0() {
    // opcode 00E0
    std::memset(gfx, 0, sizeof(gfx));
}
