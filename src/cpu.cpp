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
        char buffer [4096];

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

    if (trace && (opcode & 0xF000) != 0x0000) {
        printf("pc: %.4X opcode: %.4X sp: %.2X ", pc, opcode, sp);
        printf("I: %.4X ", I);
        printf("%s", "registers: ");
        for (int i = 0; i < 15; i++)
            printf("%.2X ", V[i]);

        printf("\n");
    }

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
            //call_subroutine_at_address_2nnn();
            break;
        case 0x3000:
            //skip_next_instruction_if_vx_equals_3xnn();
            pc += 2;
            break;
        case 0x4000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0x5000:
            //skip_next_instruction_if_vx_equals_vy();
            pc += 2;
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
                    //set_vx_to_vx();
                    break;
                case 0x0001:
                    //set_vx_to_vx_or_vx();
                    break;
                case 0x0002:
                    //set_vx_to_vx_and_vx();
                    break;
                case 0x0003:
                    //set_vx_to_vx_xor_vx();
                    break;
                case 0x0004:
                    //add_vx_to_vx();
                    break;
                case 0x0005:
                    //subtract_vx_from_vx();
                    break;
                case 0x0006:
                    shift_vx_right_by_one_8xy6();
                    break;
                case 0x0007:
                    //set_vx_to_vx_minus_vx();
                    break;
                case 0x000E:
                    shift_vx_left_by_one_8xye();
                    break;
                default:
                    break;
            }
            break;
        case 0x9000:
            //skip_next_instruction_if_vx_not_equals_vy();
            pc += 2;
            break;
        case 0xA000:
            set_i_to_address_annn();
            break;
        case 0xB000:
            //jump_to_address_annn_plus_v0();
            break;
        case 0xC000:
            //set_vx_to_random_number_and_bit_mask_nnn();
            break;
        case 0xD000:
            draw_dxyn();
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    //skip_next_instruction_if_key_pressed_vx();
                    pc += 2;
                    break;
                case 0x00A1:
                    //skip_next_instruction_if_key_not_pressed_vx();
                    pc += 2;
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    //set_vx_to_delay_timer();
                    break;
                case 0x000A:
                    //wait_for_key_press_and_store_in_vx();
                    break;
                case 0x0015:
                    //set_delay_timer_to_vx();
                    break;
                case 0x0018:
                    //set_sound_timer_to_vx();
                    break;
                case 0x001E:
                    //add_vx_to_i();
                    break;
                case 0x0029:
                    //set_i_to_sprite_location_of_vx();
                    break;
                case 0x0033:
                    //store_bcd_representation_of_vx_in_i_i_plus_1_and_i_i_plus_2();
                    break;
                case 0x0055:
                    //store_registers_in_memory_starting_at_i();
                    break;
                case 0x0065:
                    //load_registers_from_memory_starting_at_i();
                    break;
                default:
                    break;
            }
            break;
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
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
    std::memset(gfx, 0, sizeof(gfx));
    draw_flag = true;
}

void CPU::set_i_to_address_annn() {
    I = opcode & 0x0FFF;
}

void CPU::call_routine_at_address_0nnn() {}

void CPU::return_from_subroutine_00ee() {
    sp--;
    pc = stack[sp];
}

void CPU::jump_to_address_1nnn() {
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
}

void CPU::call_subroutine_at_address_2nnn() {

}

void CPU::set_vx_to_nn_6xnn() {
    V[opcode & 0x0F00] = opcode & 0x00FF;
}

void CPU::add_nn_to_vx_7xnn() {
    V[opcode & 0x0F00] += opcode & 0x00FF;
}

void CPU::draw_dxyn() {
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded
    // starting from memory location I; I value does not change after the execution of this instruction.
    // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
    // and to 0 if that does not happen
    int x = V[opcode & 0x0F00 >> 8];
    int y = V[opcode & 0x00F0 >> 4];
    int height = (opcode & 0x000F);

    for (int i = 0; i < height; i++) {
        for (int j = -4; i < 4; i++) {

        }
    }

    draw_flag = true;
}

void CPU::shift_vx_left_by_one_8xye() {
    V[opcode & 0x0F00] <<= 1;
}

void CPU::shift_vx_right_by_one_8xy6() {
    V[opcode & 0x0F00] >>= 1;
}
