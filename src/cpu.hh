#include <string>
#include <array>
#include "display.hh"

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
struct CPU {
    Display* display;
    std::array<uint16_t, 16>   stack;
    std::array<uint8_t, 16>    V;
    uint16_t pc; 
    uint16_t I;  
    uint16_t opcode;
    uint16_t cycles = 0;
    uint8_t  scale = 20;
    uint8_t  sp;
    uint8_t  memory[4096];
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    uint8_t  keypad[16];   
    bool     debug = true;
    bool     trace = false;
    bool     pause_execution = true;
    bool     waiting = false;
    bool     load_game(const std::string& name);
    void     init(Display& d);
    void     log(const char* input, ...) const;
    void     single_cycle();
    void     update_timers();

private:
    void toggle_pause();
    void Table0();
	void Table8();
	void TableE();
	void TableF();

	void op_NULL();
	void clear_display();
	void return_from_subroutine();
	void jump(uint16_t nnn);
	void call_subroutine(uint16_t nnn);
	void skip_if_equal_nn(uint16_t vx, uint8_t nn);
	void skip_if_not_equal(uint16_t vx, uint8_t nn);
	void skip_if_equal_vy(uint16_t vx, uint16_t vy);
	void set_vx(uint16_t vx, uint8_t nn);
	void add(uint16_t vx, uint8_t nn);
	void set_vx_vy(uint16_t vx, uint16_t vy);
	void vx_or(uint16_t vx, uint16_t vy);
	void vx_and(uint16_t vx, uint16_t vy);
	void vx_xor(uint16_t vx, uint16_t vy);
	void add_with_carry(uint16_t vx, uint16_t vy);
	void sub_with_borrow(uint16_t vx, uint16_t vy);
	void shift_right(uint16_t vx);
	void reverse_sub(uint16_t vx, uint16_t vy);
	void shift_left(uint16_t vx);
	void skip_if_not_equal(uint16_t vx, uint16_t vy);
	void set_index(uint16_t nnn);
	void jump_to_offset(uint16_t nnn);
	void vx_rand(uint16_t vx, uint8_t nn);
	void draw(uint16_t vx, uint16_t vy, uint8_t n);
	void skip_if_pressed(uint16_t vx);
	void skip_if_not_pressed(uint16_t vx);
	void set_vx_to_delay(uint16_t vx);
	void wait_for_key(uint16_t vx);
	void set_delay(uint16_t vx);
	void set_sound(uint16_t vx);
	void add_index(uint16_t vx);
	void set_index(uint16_t vx);
	void vx_binary(uint16_t vx);
	void store(uint16_t vx);
	void read(uint16_t vx);

    typedef void (CPU::*Instruction)();
	Instruction table[0xF + 1];
	Instruction table0[0xE + 1];
	Instruction table8[0xE + 1];
	Instruction tableE[0xE + 1];
	Instruction tableF[0x65 + 1];
};
