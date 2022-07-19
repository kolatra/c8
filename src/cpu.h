#ifndef UNTITLED_CPU_H
#define UNTITLED_CPU_H
#include <string>

// Memory Map:
// +---------------+= 0xFFF (4095) End of Chip-8 RAM
// |               |
// |               |
// |               |
// |               |
// |               |
// | 0x200 to 0xFFF|
// |     Chip-8    |
// | Program / Data|
// |     Space     |
// |               |
// |               |
// |               |
// +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
// |               |
// |               |
// |               |
// +---------------+= 0x200 (512) Start of most Chip-8 programs
// | 0x000 to 0x1FF|
// | Reserved for  |
// |  interpreter  |
// +---------------+= 0x000 (0) Start of Chip-8 RAM
class CPU {
    public:

    unsigned char fontset[80] = {
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

    uint16_t pc; // program counter
    uint16_t I;  // index register
    uint16_t stack[16];
    uint16_t opcode;
    uint8_t  sp;
    uint8_t  V[16]; // 8-bit v0-vF registers
    uint8_t  memory[4096];
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    int      gfx[64 * 32]; // 2048 pixels in total on screen
    int      keypad[16];   // values are 0-F
    bool     draw_flag;
    bool     trace = true;
    bool     pause_execution = false;
    void     init();
    bool     load_game(const char* name);
    void     single_cycle();
    void     set_keys();


private: // TODO make all of these names better
    void clear_display();
    void return_from_subroutine_00ee();
    void jump_to_address_1nnn();
    void call_subroutine_at_address_2nnn();
    void skip_next_instruction_if_vx_equals_3xnn();
    void skip_next_instruction_if_vx_not_equals_4xnn();
    void skip_next_instruction_if_vx_equals_5xy0();
    void set_vx_to_nn_6xnn();
    void add_nn_to_vx_7xnn();
    void set_vx();
    void or_vx_vy();
    void and_vx_vy();
    void xor_vx_vy();
    void add_vx_vy(); // vf is set to 1 when there's a carry, and 0 otherwise
    void subtract_vx_vy(); // vf is set to 0 when there's a borrow, and 1 otherwise
    void shift_vx_right_by_one_8xy6(); // vf is set to the value of the least significant bit of vx before the shift
    void set_vx_to_vy_minus_vx_8xy7(); // vf is set to 0 when there's a borrow, and 1 otherwise
    void shift_vx_left_by_one_8xye(); // vf is set to the value of the most significant bit of vx before the shift
    void skip_next_instruction_if_vx_equals_vy_9xy0();
    void set_i_to_address_annn();
    void jump_to_nnn_v0();
    void rand_vx();
    void draw_dxyn();
    void skip_next_instruction_if_key_pressed();
    void skip_next_instruction_if_key_not_pressed();
    void set_vx_to_delay_timer();
    void get_key();
    void set_delay_timer_to_vx();
    void set_sound_timer_to_vx();
    void add_i_and_vx();
    void get_sprite_from_vx();
    void store_bcd_number();
    void copy_reg();
    void read_reg();
};

#endif //UNTITLED_CPU_H
