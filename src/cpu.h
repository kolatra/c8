#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions" // TODO remove
#ifndef UNTITLED_CPU_H
#define UNTITLED_CPU_H
#include <string>

/**
* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
* 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
* 0x200-0xFFF - Program ROM and work RAM
*/

class CPU {
    public:

    bool           drawFlag;
    unsigned char  memory[4096];  // 4k memory in total on system
    unsigned char  V[16]; // there are 15 8-bit general V named v0-vE, the 16th register is the "carry flag"
    unsigned char  gfx[64 * 32];
    unsigned char  delay_timer;
    unsigned char  sound_timer;
    unsigned char  keypad[16];    // values are 0-F
    unsigned short rate = 60;     // the timers use a consistent rate of 60hz
    unsigned short opcode;        // 35 total opcodes, all are 2 bytes long. unsigned short is 2 bytes long
    unsigned short I;         // I register
    unsigned short pc;            // program counter
    // store the program counter in the stack before performing a jump
    // or calling a subroutine.
    // use the stack pointer to remember which level of the stack is used
    unsigned short stack[16];
    unsigned short sp;
    unsigned short fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    void init();
    void load_game(std::string const &name);
    void lie_to_the_people();
    void set_keys();
    void next(bool skip);
    void skip();

    void call_routine_at_address_0nnn();
    void clear_display_00e0();
    void return_from_subroutine_00ee();
    void jump_to_address_1nnn();
    void call_subroutine_at_address_2nnn();
    void skip_next_instruction_if_vx_equals_3xnn();
    void skip_next_instruction_if_vx_not_equals_4xnn();
    void skip_next_instruction_if_vx_equals_5xy0();
    void skip_next_instruction_if_vx_not_equals_6xnn();
    void set_vx_to_nn_7xnn();
    void add_nn_to_vx_8xy0();
    void set_vx_to_vx_or_vy_8xy1();
    void set_vx_to_vx_and_vy_8xy2();
    void set_vx_to_vx_xor_vy_8xy3();
    void add_vy_to_vx_8xy4(); // vf is set to 1 when there's a carry, and 0 otherwise
    void subtract_vy_from_vx_8xy5(); // vf is set to 0 when there's a borrow, and 1 otherwise
    void shift_vx_right_by_one_8xy6(); // vf is set to the value of the least significant bit of vx before the shift
    void set_vx_to_vy_minus_vx_8xy7(); // vf is set to 0 when there's a borrow, and 1 otherwise
    void shift_vx_left_by_one_8xye(); // vf is set to the value of the most significant bit of vx before the shift
    void skip_next_instruction_if_vx_equals_vy_9xy0();
    void set_i_to_address_annn();
    void jump_to_address_plus_v0_bnnn();
    void set_vx_bitwise_and_random_cxnn();
    void draw_sprite_dxyn();
    void skip_next_if_key_pressed_ex9e();
    void skip_next_if_key_not_pressed_exa1();
    void set_vx_to_delay_fx07();
    void block_until_key_pressed_fx0a();
    void set_delay_timer_to_vx_fx15();
    void set_sound_timer_to_vx_fx18();
    void add_vx_to_i_fx1e();
    void set_i_to_sprite_location_fx29();
    void set_bcd_of_vx_fx33();
    void set_vx_to_delay_fx55();
    void fill_fx65();
};

#endif //UNTITLED_CPU_H

#pragma clang diagnostic pop