#pragma once

#include <SDL.h>
#include "cpu.h"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class Display {
public:
    Display(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Display();
    void update(void const* buffer, int pitch, CPU cpu);
    void handle_input(uint8_t* keys);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};
