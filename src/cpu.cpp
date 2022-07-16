#include <iostream>
#include <cstring>
#include "cpu.h"

const unsigned int FONTSET_START = 0x50;

void CPU::init() {
    pc     = 0x200;
    opcode = 0;
    I      = 0;
    sp     = 0;

    // Clear display
    // Clear stack
    // Clear V V0-VF
    // Clear memory

    for (int i = 0; i < 80; ++i) { // 0-80 is reserved in memory for fonts
        memory[FONTSET_START + i] = fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

void CPU::load_game(std::string const &name) {

}

void CPU::lie_to_the_people() {
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
    opcode = memory[pc] << 8 | memory[pc + 1];
    std::cout << opcode << '\n';


    // parse opcode and execute
    switch (opcode & 0xF000) {
        case 0x00E0:
            clear_display_00e0();
            break;
        case 0x00EE:
            return_from_subroutine_00ee();
            break;
        case 0x1000:
            jump_to_address_1nnn();
            break;
        case 0x2000:
            call_subroutine_at_address_2nnn();
            break;
        case 0x3000:
            skip_next_instruction_if_vx_equals_3xnn();
            break;
        case 0xA000:
            set_i_to_address_annn();
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

void CPU::next(bool skip) {
    if (!skip) {
        pc += 2;
    } else {
        pc += 4;
    }
}

void CPU::set_keys() {

}

void CPU::clear_display_00e0() {
    // opcode 00E0
    std::memset(gfx, 0, sizeof(gfx));
}

void CPU::set_i_to_address_annn() {
    I = opcode & 0x0FFF;
}
