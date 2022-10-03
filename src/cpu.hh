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
    //std::array<uint16_t, 2048> m_gfx;
    std::array<uint16_t, 16>   m_stack;
    std::array<uint8_t, 16>    m_registers;
    //uint16_t m_gfx[2048];
    uint16_t m_eip; 
    uint16_t m_index;  
    uint16_t m_opcode;
    uint16_t m_cycles = 0;
    uint8_t  m_scale = 20;
    uint8_t  m_esp;
    uint8_t  m_ram[4096];
    uint8_t  m_delay_timer;
    uint8_t  m_sound_timer;
    uint8_t  m_keypad[16];   
    bool     m_debug = true;
    bool     m_trace = false;
    bool     m_pause_execution = true;
    bool     load_game(const std::string& name);
    void     init(Display& d);
    void     log(const char* input, ...) const;
    void     single_cycle();
    void     update_timers();

private:
    void toggle_pause();
};
