#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

struct {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 0};
    SDL_Color blue = {0, 0, 255, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color violet = {238, 130, 238, 255};
    SDL_Color pink = {255, 192, 203, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color orange = {255, 165, 0, 255};
    SDL_Color cyan = {0, 255, 255, 255};
} Color;


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

    void SetBackgroundColor(SDL_Color & color);
};