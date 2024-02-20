#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

class Render{
public:
    //renderer flags
    Uint32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    SDL_Renderer * renderer = NULL;

    //method
    Render(SDL_Window * window);
    void Clear();
    void Display();
};