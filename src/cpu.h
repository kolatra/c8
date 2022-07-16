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

    unsigned char  memory[4096];  // 4k memory in total on system
    unsigned char  registers[16]; // there are 15 8-bit general registers named v0-vE, the 16th register is the "carry flag"
    unsigned char  gfx[64 * 32];
    unsigned char  delay_timer;
    unsigned char  sound_timer;
    unsigned char  keypad[16];    // values are 0-F
    unsigned int   rate = 60;     // the timers use a consistent rate of 60hz
    unsigned short opcode;        // 35 total opcodes, all are 2 bytes long. unsigned short is 2 bytes long
    unsigned short index;         // index register
    unsigned short pc;            // program counter
    // store the program counter in the stack before performing a jump
    // or calling a subroutine.
    // use the stack pointer to remember which level of the stack is used
    unsigned short stack[16];
    unsigned short sp;
    bool           drawFlag;

    void init();
    void load_game(std::string const &name);
    void pretend_cycle();
    void set_keys();
    void clear_display();
};

#endif //UNTITLED_CPU_H
