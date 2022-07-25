#pragma once

#include <SDL.h>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class Platform {
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void update(void const* buffer, int pitch);
    void handle_input(uint8_t* keys);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};
