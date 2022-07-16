#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include "cpu.h"

void CPU::init() {
    pc     = 0x200;
    opcode = 0;
    I      = 0;
    sp     = 0;

    memset(gfx, 0, sizeof(gfx));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(memory, 0, sizeof(memory));

    for (int i = 0; i < 80; ++i) {
        memory[0x50 + i] = fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

bool CPU::load_game(const char* name) {
    std::ifstream file(name, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (int i = 0; i < size; ++i) {
            memory[0x200 + i] = buffer[i];
        }
        delete[] buffer;

        return true;
    } else {
        return false;
    }
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
    pc += 2;

    if (trace) {
        if (opcode != 0x0000) {
            printf("pc: %.4X opcode: %.4X sp: %.2X ", pc, opcode, sp);
            printf("I: %.4X ", I);
            printf("%s", "registers: ");
            for (int i = 0; i < 15; i++)
                printf("%.2X ", V[i]);

            printf("\n");
        }
    }

    // parse opcode and execute
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0:
                    clear_display();
                    break;
                case 0x00EE:
                    // return from subroutine
                    break;
                default:
                    pc += 2;
                    break;
            }
        case 0x1000:
            //jump_to_address_1nnn();
            break;
        case 0x2000:
            //call_subroutine_at_address_2nnn();
            break;
        case 0x3000:
            //skip_next_instruction_if_vx_equals_3xnn();
            break;
        case 0xA000:
            //set_i_to_address_annn();
            break;
        default:
            //printf("Unknown opcode: 0x%X\n", opcode);
            break;
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

void CPU::clear_display() {
    // opcode 00E0
    std::memset(gfx, 0, sizeof(gfx));
    draw_flag = true;
    pc += 2;
}

void CPU::set_i_to_address_annn() {
    I = opcode & 0x0FFF;
    pc += 2;
}

void CPU::call_routine_at_address_0nnn() {
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
}
