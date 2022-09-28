#include "display.hh"
#include <SDL.h>

Display::Display(char const* title) {
    window_ = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        m_video_width * m_pixel_size, 
        m_video_height * m_pixel_size, 
        SDL_WINDOW_SHOWN
    );
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    rect_.x = 0;
    rect_.y = 0;
    rect_.w = m_video_width;
    rect_.h = m_video_height;
}

Display::~Display() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
}

void Display::clear() {
    mem.reset();
    update();
}


void Display::update() {
    if (!m_draw_flag)
        return;

    SDL_SetRenderDrawColor(renderer_, 35, 35, 35, 255);
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(renderer_, 35, 35, 35, 255);
    SDL_Rect clear = {
        rect_.x * m_pixel_size,
        rect_.y * m_pixel_size,
        rect_.w * m_pixel_size,
        rect_.h * m_pixel_size
    };
    SDL_RenderFillRect(renderer_, &clear);


    SDL_SetRenderDrawColor(renderer_, 200, 200, 200, 255);
    for(auto i = 0; i < rect_.w * rect_.h; i++) {

        uint16_t pos_x = rect_.x + i % rect_.w;
        uint16_t pos_y = (rect_.y + i / rect_.w) * m_video_width;

        uint16_t pos = pos_x + pos_y;

        if(mem[pos]) {
            SDL_Rect r = { //construct "pixel" rect
                rect_.x * m_pixel_size + (i % rect_.w) * m_pixel_size,
                rect_.y * m_pixel_size + i / rect_.w * m_pixel_size,
                m_pixel_size,
                m_pixel_size
            };
            SDL_RenderFillRect(renderer_, &r);
        }
    }
    SDL_RenderPresent(renderer_);
}

void Display::handle_input(uint8_t* keys) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

    }
}

uint8_t Display::set(uint8_t *src, uint8_t range, uint16_t x, uint16_t y) {
    bool ret = false;
    for (auto row = 0; row < range; row++) {
        auto byte = src[row];
        for (auto bit = 0; bit < 8; bit++) {
            if (byte & 0x80) {
                while(x > m_video_width - 1) x -= m_video_width;
                while(x < 0) x += m_video_width;
                while(y > m_video_height - 1) y -= m_video_height;
                while(y < 0) y += m_video_height;
                size_t pos = y * m_video_width + x;
                mem.flip(pos);
                ret = !mem[pos];
            }
            byte = byte << 1;
        }
    }
    update();
    return ret;
}

