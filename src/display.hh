#pragma once

#include <SDL.h>
#include <bitset>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Display {
public:
    Display(char const* title);
    ~Display();
    void update();
    void handle_input(uint8_t* keys);
    void clear();
    uint16_t m_video_height = 32;
    uint16_t m_video_width  = 64;
    uint16_t m_pixel_size   = 12;
    std::bitset<2048> mem;
    uint8_t set(uint8_t *src, uint8_t range, uint16_t x, uint16_t y);
    bool m_draw_flag = false;
    // move CPU::gfx to here and pass values to the display from the cpu instead of directly on it?

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Rect rect_;
};