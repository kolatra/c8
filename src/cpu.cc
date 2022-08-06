#include <iostream>
#include <cstring>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include "cpu.hh"

void CPU::log(const char* input, ...) const {
    if (this->debug)  {
        va_list args;
        va_start(args, input);
        vprintf(input, args);
        va_end(args);
    }
}

void CPU::init() {
    m_pc     = 0x200;
    m_opcode = 0x0;
    m_index      = 0x0;
    m_sp     = 0x0;

    memset(&m_gfx, 0, sizeof(m_gfx));
    memset(&m_v, 0, sizeof(m_v));
    memset(&m_stack, 0, sizeof(m_stack));
    memset(&m_ram, 0, sizeof(m_ram));

    std::vector<uint8_t> m_fontset = {
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

    for (int i = 0; i < m_fontset.size(); ++i) {
        m_ram[0x50 + i] = m_fontset[i];
    }

    m_delay_timer = 0;
    m_sound_timer = 0;
}

void CPU::toggle_pause() {
    pause_execution ^= true;
}

bool CPU::load_game(const std::string &name) {
    std::ifstream file(name, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        char buffer[4096];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (int i = 0; i < size; ++i) {
            m_ram[0x200 + i] = buffer[i];
        }

        return true;
    } else {
        return false;
    }
}

void CPU::single_cycle() {
    m_cycles++;
    m_opcode = m_ram[m_pc] << 8 | m_ram[m_pc + 1];

    uint8_t  vx  = (m_opcode & 0x0f00) >> 8;
    uint8_t  vy  = (m_opcode & 0x00f0) >> 4;
    uint8_t  nnn = (m_opcode & 0x0fff)     ;
    uint8_t  nn  = (m_opcode & 0x00ff)     ;
    uint8_t  n   = (m_opcode & 0x000f)     ;
    uint32_t result;
    bool     waiting;

    if (trace && (m_opcode & 0xFFFF) != 0x0000) {
        printf("%d: ", m_cycles);
        printf("m_pc: %.4X / m_opcode: %.4X / m_sp: %.2X ", m_pc, m_opcode, m_sp);
        printf("/ index: %.4X ", m_index);
        printf("%s", "/ registers: ");
        for (int i = 0; i < 15; i++)
            printf("%.2X ", m_v[i]);
        printf("\n");
    }

    // parse opcode and execute
    switch (m_opcode & 0xF000) {
        case 0x0000:
            switch (m_opcode & 0x00FF) {
                case 0x00E0:
                    std::memset(&m_gfx, 0, sizeof(m_gfx));
                    draw_flag = true;
                    m_pc += 2;
                    break;
                case 0x00EE:
                    m_pc = m_stack[m_sp];
                    m_sp--;
                    m_pc += 2;
                    break;
                default:
                    break;
            }
            break;
        case 0x1000:
            m_pc = nnn;
            break;
        case 0x2000:
            m_stack[m_sp] = m_pc;
            m_sp++;
            m_pc = nnn;
            break;
        case 0x3000:
            if (m_v[vx] == nn)
                m_pc += 4;
            break;
        case 0x4000:
            if (m_v[vx] != nn)
                m_pc += 4;
            break;
        case 0x5000:
            if (m_v[vx] == m_v[nn])
                m_pc += 4;
            break;
        case 0x6000:
            log("m_opcode: 0x%X\n", m_opcode);
            log("Vx:     %d (0x%X)\n", vx, vx);
            log("Vx:     %s\n", std::bitset<16>(vx).to_string().c_str());
            log("nn:     %d (0x%X)\n", nn, nn);
            log("nn:     %s\n", std::bitset<16>(nn).to_string().c_str());
            m_v[vx] = nn;
            m_pc += 2;
            break;
        case 0x7000:
            m_v[m_opcode & 0x0F00 >> 8] += m_opcode & 0x00FF;
            m_pc += 2;
            break;
        case 0x8000:
            switch (m_opcode & 0x000F) {
                case 0x0000:
                    m_v[m_opcode & 0x0F00 >> 8] = m_v[m_opcode & 0x00F0 >> 4];
                    m_pc += 2;
                    break;
                case 0x0001:
                    m_v[m_opcode & 0x0F00 >> 8] |= m_v[m_opcode & 0x00F0 >> 4];
                    m_pc += 2;
                    break;
                case 0x0002:
                    m_v[m_opcode & 0x0F00 >> 8] &= m_v[m_opcode & 0x00F0 >> 4];
                    m_pc += 2;
                    break;
                case 0x0003:
                    m_v[m_opcode & 0x0F00 >> 8] ^= m_v[m_opcode & 0x00F0 >> 4];
                    m_pc += 2;
                    break;
                case 0x0004:
                    result = vx+vy;
                    m_v[0xf] = result > 0xffff;
                    m_v[vx] = result & 0xff;
                    m_pc += 2;
                    break;
                case 0x0005:
                    result = vx-vy;
                    m_v[0xf] = m_v[vx] > m_v[vy];
                    m_v[vx] = result;
                    m_pc += 2;
                    break;
                case 0x0006:
                    m_v[0xf] = (m_v[vx] & 0x1);
                    m_v[vx] >>= 1;
                    m_pc += 2;
                    break;
                case 0x0007:
                    result = vy - vx;
                    m_v[0xf] = m_v[vy] > m_v[vx];
                    m_v[vx] = result;
                    m_pc += 2;
                    break;
                case 0x000E: {
                    std::bitset<32> bs(vx);
                    m_v[0xf] = bs[bs.size() - 1];
                    m_v[vx] <<= 1;
                    m_pc += 2;
                    break;
                }
                default:
                    log("Unknown m_opcode: 0x%X\n", m_opcode);
                    break;
            }
            break;
        case 0x9000:
            if (m_v[vx] == m_v[vy])
                m_pc += 4;
            break;
        case 0xA000:
            m_index = nnn;
            m_pc += 2;
            break;
        case 0xB000:
            m_pc = nnn + m_v[0];
            m_pc += 2;
            break;
        case 0xC000:
            m_v[vx] = (rand() % 256) & nn;
            m_pc += 2;
            break;
        case 0xD000: {
            /*std::cout << "m: " << m << std::hex;
            std::cout << std::bitset<8>(m) << '\n';
            std::cout << "n: " << n << std::hex;
            std::cout << std::bitset<8>(n) << '\n';*/

            /*uint8_t Vx = m_v[m_opcode & 0x0F00 >> 8];
            uint8_t Vy = m_v[m_opcode & 0x00F0 >> 4];
            uint8_t height = (m_opcode & 0x000F);

            uint8_t x = Vx & m_video_width;
            uint8_t y = Vy & m_video_height;*/

            uint8_t height = n;
            uint8_t xPos = m_v[vx] % m_video_width;
            uint8_t yPos = m_v[vy] % m_video_height;
            log("m_opcode: 0x%X\n", m_opcode);
            log("Drawing at x: %d, y: %d\n", xPos, yPos);
            log("Drawing height: %d\n", height);
            log("Vx: %d\n", vx);
            log("Vy: %d\n", vy);

            m_v[0xf] = 0;

            for (auto row = 0; row < height; row++) {
                uint8_t byte = m_ram[m_index + row];
                for (auto column = 0; column < 8; column++) {
                    uint8_t pixel = byte & (0x80 >> column);
                    auto* screen_pixel = reinterpret_cast<uint32_t *>(&m_gfx[(yPos + row) * m_video_width + (xPos + column)]);
                    if (pixel) {
                        if (*screen_pixel == 0xFFFFFFFF) m_v[0xf] = 1;
                        *screen_pixel ^= 0xFFFFFFFF;
                    }
                }
            }

            draw_flag = true;
            m_pc += 2;
            break;
        }
        case 0xE000:
            switch (m_opcode & 0x00FF) {
                case 0x009E:
                    if (!keypad[vx])
                        m_pc += 4;
                    break;
                case 0x00A1:
                    if (!keypad[vx])
                        m_pc += 4;
                    break;
                default:
                    log("Unknown m_opcode: 0x%X\n", m_opcode);
                    break;
            }
            break;
        case 0xF000:
            switch (m_opcode & 0x00FF) {
                case 0x0007:
                    m_v[m_opcode & 0x0F00 >> 8] = m_delay_timer;
                    m_pc += 2;
                    break;
                case 0x000A:
                    waiting = true;
                    for (int i = 0; i < 0xf; i++) {
                        if (keypad[i]) {
                            m_v[vx] = i;
                            waiting = false;
                        }
                    }
                    if (waiting)
                        m_pc -= 2;

                    m_pc += 2;
                    break;
                case 0x0015:
                    m_delay_timer = m_v[m_opcode & 0x0F00];
                    m_pc += 2;
                    break;
                case 0x0018:
                    m_sound_timer = m_v[m_opcode & 0x0F00];
                    m_pc += 2;
                    break;
                case 0x001E:
                    m_index += m_v[m_opcode & 0x0F00];
                    m_pc += 2;
                    break;
                case 0x0029:
                    m_index = m_ram[0x50 + m_v[m_opcode & 0x0F00]];
                    m_pc += 2;
                    break;
                case 0x0033:
                    m_ram[m_index]   = (vx / 100) % 10;
                    m_ram[m_index + 1] = (vx / 10) % 10;
                    m_ram[m_index + 2] = vx % 10;
                    m_pc += 2;
                    break;
                case 0x0055:
                    // Store registers V0 through Vx in m_ram starting at location m_index.
                    // The interpreter copies the values of registers V0 through Vx into m_ram, starting at the address in m_index.
                    for (int i = 0; i < vx; i++) {
                        uint16_t addr = m_index + i;
                        memset(&addr, m_v[i], sizeof(m_v[i]));
                    }
                    m_pc += 2;
                    break;
                case 0x0065:
                    // Read registers V0 through Vx from m_ram starting at location m_index.
                    // The interpreter reads values from m_ram starting at location m_index into registers V0 through Vx.
                    for (int i = 0; i < vx; i++) {
                        uint16_t addr = m_index + i;
                        memset(&m_v[i], addr, sizeof(addr));
                    }
                    m_pc += 2;
                    break;
                default:
                    log("Unknown m_opcode: 0x%X\n", m_opcode);
                    break;
            }
            break;
        default:
            log("Unknown m_opcode: 0x%X\n", m_opcode);
            break;
    }

    if (m_delay_timer > 0)
        --m_delay_timer;

    if (m_sound_timer > 0) {
        log("BEEP!\n");
        --m_sound_timer;
    }
}
