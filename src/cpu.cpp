#include <iostream>
#include "cpu.h"

void CPU::init() {
    pc     = 0x200;
    opcode = 0;
    index  = 0;
    sp     = 0;

    // Clear display
    // Clear stack
    // Clear registers V0-VF
    // Clear memory
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
    switch (opcode) {
        case 0xA2F0:
            std::cout << "opcode is 0xA2F0" << std::endl;
            index = opcode & 0x0FFF;
            pc += 2; // maybe just do this under the switch? if the next opcode should be skipped, increment by 4
            break;
        case 0x00E0:
            clear_display();
            break;
        default:
            std::cout << "unknown opcode! " << opcode << '\n';
            break;
    }
    // update timers
}

void CPU::set_keys() {

}

void CPU::clear_display() {
    // opcode 00E0
}
