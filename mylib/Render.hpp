#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

struct {
    SDL_Rect tex256 = {0, 0, 90, 160};
} TexRect;

class Render{
public:
    Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    SDL_Renderer * renderer = nullptr;

    void Init(SDL_Window * window);
    void Clear();
    void Display();
};