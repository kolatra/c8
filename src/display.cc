#include "display.hh"
#include <SDL.h>
#include "cpu.hh"

Display::Display(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Display::~Display() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// is the type on this wrong?
// should it be a pointer?
// okay so far i've tried changing this type from void const* to uint16_t* (which seemed to have no effect)
// i don't think the screen is clearing properly, because it sits on the black screen
// with the final 8 bits(pixels) set for the entire instruction set
void Display::update(void const* buffer, int pitch, const CPU& cpu) {
    if (!cpu.draw_flag) return;
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Display::handle_input(uint8_t *keys) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

    }
}
