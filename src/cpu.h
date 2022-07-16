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
    unsigned char  registers[16]; // there are 15 8-bit general registers named v0-vE, the 16th register is the "carry flag"
    unsigned char  gfx[64 * 32];
    unsigned char  delay_timer;
    unsigned char  sound_timer;
    unsigned char  keypad[16];    // values are 0-F
    unsigned short rate = 60;     // the timers use a consistent rate of 60hz
    unsigned short opcode;        // 35 total opcodes, all are 2 bytes long. unsigned short is 2 bytes long
    unsigned short index;         // index register
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
    void pretend_cycle();
    void set_keys();

    void call_routine_at_address_0nnn();
    void clear_display_00e0();
};

#endif //UNTITLED_CPU_H

#pragma clang diagnostic pop