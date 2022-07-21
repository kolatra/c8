#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include "cpu.h"

void CPU::init() {
    pc     = 0x200;
    opcode = 0;
    I      = 0;
    sp     = 0;

    memset(gfx,    0, sizeof(gfx));
    memset(V,      0, sizeof(V));
    memset(stack,  0, sizeof(stack));
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
        char buffer[4096];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (int i = 0; i < size; ++i) {
            memory[0x200 + i] = buffer[i];
        }

        return true;
    } else {
        return false;
    }
}

void CPU::single_cycle() {
    /**
     * fetch opcode from the memory at the location stored in the program counter
     * data is stored in an array in which each address contains one byte
     *
     * in the assumption of
     * memory[pc]     == 0xA2
     * memory[pc + 1] == 0xF0
     *
     * 0xA2 was shifted left 8 bits, which adds 8 zeroes,
     * and increases the byte address by 1
     *
     * 0xA2       HEX
     * 1010 0010  BIN
     * 0xF0       HEX
     * 1111 0000  BIN
     *
     * 0xA2   << 8    = 0xA200   1010 0010 0000 0000
     * 0xA200 OR 0xF0 = 0xA2F0   1010 0010 1111 0000
     */
    opcode = memory[pc] << 8 | memory[pc + 1];

    pc += 2;

    // parse opcode and execute
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0:
                    clear_display();
                    break;

                case 0x00EE:
                    return_from_subroutine_00ee();
                    break;

                default:
                    break;
            }
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

        case 0x4000:
            skip_next_instruction_if_vx_not_equals_4xnn();
            break;

        case 0x5000:
            skip_next_instruction_if_vx_equals_5xy0();
            break;

        case 0x6000:
            set_vx_to_nn_6xnn();
            break;

        case 0x7000:
            add_nn_to_vx_7xnn();
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    set_vx();
                    break;

                case 0x0001:
                    or_vx_vy();
                    break;

                case 0x0002:
                    and_vx_vy();
                    break;

                case 0x0003:
                    xor_vx_vy();
                    break;

                case 0x0004:
                    add_vx_vy();
                    break;

                case 0x0005:
                    subtract_vx_vy();
                    break;

                case 0x0006:
                    shift_vx_right_by_one_8xy6();
                    break;

                case 0x0007:
                    set_vx_to_vy_minus_vx_8xy7();
                    break;

                case 0x000E:
                    shift_vx_left_by_one_8xye();
                    break;

                default:
                    break;

            }
            break;

        case 0x9000:
            skip_next_instruction_if_vx_equals_vy_9xy0();
            break;

        case 0xA000:
            set_i_to_address_annn();
            break;

        case 0xB000:
            jump_to_nnn_v0();
            break;

        case 0xC000:
            rand_vx();
            break;

        case 0xD000:
            draw_dxyn();
            break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    skip_next_instruction_if_key_pressed();
                    break;
                case 0x00A1:
                    skip_next_instruction_if_key_not_pressed();
                    break;
                default:
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    set_vx_to_delay_timer();
                    break;

                case 0x000A:
                    get_key();
                    break;

                case 0x0015:
                    set_delay_timer_to_vx();
                    break;

                case 0x0018:
                    set_sound_timer_to_vx();
                    break;

                case 0x001E:
                    add_i_and_vx();
                    break;

                case 0x0029:
                    get_sprite_from_vx();
                    break;

                case 0x0033:
                    store_bcd_number();
                    break;

                case 0x0055:
                    copy_reg();
                    break;

                case 0x0065:
                    read_reg();
                    break;

                default:
                    break;
            }
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }

    if (trace && (opcode & 0xFFFF) != 0x0000) {
        printf("pc: %.4X / opcode: %.4X / sp: %.2X ", pc, opcode, sp);
        printf("/ index: %.4X ", I);
        printf("%s", "/ registers: ");
        for (int i = 0; i < 15; i++)
            printf("%.2X ", V[i]);

        printf("\n");
    }

    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        printf("BEEP!\n");
        --sound_timer;
    }
}

void CPU::set_keys() {

}

void CPU::clear_display() {
    std::memset(gfx, 0, sizeof(gfx));
    draw_flag = true;
}

void CPU::return_from_subroutine_00ee() {
    pc = stack[sp];
    sp--;
}

void CPU::jump_to_address_1nnn() {
    pc = opcode & 0x0FFF;
}

void CPU::call_subroutine_at_address_2nnn() {
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
}

void CPU::skip_next_instruction_if_vx_equals_3xnn() {
    int vx = (opcode&0x0F00) >> 8;
    int nn = (opcode&0x00FF);
    if (V[vx] == nn)
        pc += 2;
}

void CPU::skip_next_instruction_if_vx_not_equals_4xnn() {
    int vx = (opcode&0x0F00) >> 8;
    int nn = (opcode&0x00FF);
    if (V[vx] != nn)
        pc += 2;
}

void CPU::skip_next_instruction_if_vx_equals_5xy0() {
    int vx = (opcode&0x0F00) >> 8;
    int nn = (opcode&0x00F0) >> 4;
    if (V[vx] == V[nn])
        pc += 2;
}

void CPU::set_vx_to_nn_6xnn() {
    V[opcode & 0x0F00 >> 8] = opcode & 0x00FF;
}

void CPU::add_nn_to_vx_7xnn() {
    V[opcode & 0x0F00 >> 8] += opcode & 0x00FF;
}

void CPU::set_vx() {
    V[opcode & 0x0F00 >> 8] = V[opcode & 0x00F0];
}

void CPU::or_vx_vy() {
    V[opcode & 0x0F00 >> 8] |= V[opcode & 0x00F0];
}

void CPU::and_vx_vy() {
    V[opcode & 0x0F00 >> 8] &= V[opcode & 0x00F0 >> 4];
}

void CPU::xor_vx_vy() {
    V[opcode & 0x0F00 >> 8] ^= V[opcode & 0x00F0 >> 4];
}

void CPU::add_vx_vy() {
    V[opcode & 0x0F00 >> 8] += V[opcode & 0x00F0 >> 4];
}

void CPU::subtract_vx_vy() {
    V[opcode & 0x0F00 >> 8] -= V[opcode & 0x00F0 >> 4];
}

void CPU::shift_vx_right_by_one_8xy6() {
    V[opcode & 0x0F00 >> 8] >>= 1;
}

void CPU::set_vx_to_vy_minus_vx_8xy7() {
    V[opcode & 0x0F00 >> 8] = V[opcode & 0x00F0 >> 4] - V[opcode & 0x0F00 >> 8];
}

void CPU::shift_vx_left_by_one_8xye() {
    V[opcode & 0x0F00 >> 8] <<= 1;
}

void CPU::skip_next_instruction_if_vx_equals_vy_9xy0() {
    if (V[opcode & 0x0F00] == V[opcode & 0x00F0 >> 4])
        pc += 2;
}

void CPU::set_i_to_address_annn() {
    I = opcode & 0x0FFF;
}

void CPU::jump_to_nnn_v0() {
    pc = (opcode & 0x0FFF) + V[0];
}

void CPU::rand_vx() {
    V[opcode & 0x0F00] = (rand() % 256) & (opcode & 0x00FF);
}

void CPU::draw_dxyn() {
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded
    // starting from memory location I; I value does not change after the execution of this instruction.
    // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
    // and to 0 if that does not happen
    int x = V[opcode & 0x0F00 >> 8] % 64;
    int y = V[opcode & 0x00F0 >> 4] % 32;
    int height = (opcode & 0x000F);

    for (int i = 0; i < height; i++) {
        for (int j = -4; i < 4; i++) {

        }
    }

    draw_flag = true;
}

void CPU::skip_next_instruction_if_key_pressed() {
    // if (key() == VX)
}

void CPU::skip_next_instruction_if_key_not_pressed() {
    // if (key() != VX)
}

void CPU::set_vx_to_delay_timer() {
    V[opcode & 0x0F00] = delay_timer;
}

void CPU::get_key() { // TODO
    int key = getchar();
    V[opcode & 0x0F00] = key;
}

void CPU::set_delay_timer_to_vx() {
    delay_timer = V[opcode & 0x0F00];
}

void CPU::set_sound_timer_to_vx() {
    sound_timer = V[opcode & 0x0F00];
}

void CPU::add_i_and_vx() {
    I += V[opcode & 0x0F00];
}

void CPU::get_sprite_from_vx() {
    I = fontset[V[opcode & 0x0F00]];
}

// TODO keep an eye on this
void CPU::store_bcd_number() {
    int num = opcode & 0x0F00;
    std::vector<std::bitset<4>> repr;
    while(num > 0){
        repr.emplace_back(num % 10);
        num /= 10;
    }
    std::reverse(repr.begin(), repr.end());
    memset(&I,   repr[0].to_ulong(), sizeof(repr[0]));
    memset(&I+1, repr[1].to_ulong(), sizeof(repr[1]));
    memset(&I+2, repr[2].to_ulong(), sizeof(repr[2]));
}

void CPU::copy_reg() {
    // Store registers V0 through Vx in memory starting at location I.
    // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
    int bound = (opcode & 0x0F00);
    for (int i = 0; i < bound; i++) {
        int addr = I + i;
        memset(&addr, V[i], sizeof(V[i]));
    }
}

void CPU::read_reg() {
    // Read registers V0 through Vx from memory starting at location I.
    // The interpreter reads values from memory starting at location I into registers V0 through Vx.
    int bound = (opcode & 0x0F00);
    for (int i = 0; i < bound; i++) {
        V[i] = I + i;
    }
}
