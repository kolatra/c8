#ifndef UNTITLED_CPU_H
#define UNTITLED_CPU_H
#include <string>
#include <vector>
#include <array>

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
    // TODO swap the arrays to vectors
    std::array<uint16_t, 64*32> m_gfx;
    std::array<uint16_t, 16>    m_stack;
    std::array<uint8_t, 16>     m_v;
    uint16_t m_pc; // program counter
    uint16_t m_index;  // index register
    uint16_t m_opcode;
    uint16_t m_video_height = 32;
    uint16_t m_video_width = 64;
    uint16_t m_cycles;
    uint8_t  m_scale = 20;
    uint8_t  m_sp;
    uint8_t  m_ram[4096];
    uint8_t  m_delay_timer;
    uint8_t  m_sound_timer;
    uint8_t  keypad[16];   // values are 0-F
    bool     debug = false;
    bool     trace = false;
    bool     pause_execution = true;
    bool     draw_flag = false;
    bool     load_game(const std::string &name);
    void     init();
    void     log(const char* s, ...) const;
    void     single_cycle();

private:
    void toggle_pause();
};

#endif //UNTITLED_CPU_H
