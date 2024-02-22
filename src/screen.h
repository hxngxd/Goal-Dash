#pragma once
#include "../include/SDL2/SDL.h"

class Screen{
private:

public:
    static const char * title;
    static int width;
    static int height;
    static int scale;

    static const int x = SDL_WINDOWPOS_CENTERED;
    static const int y = SDL_WINDOWPOS_CENTERED;

    static const Uint32 fixedsize = 0;
    static const Uint32 resizable = SDL_WINDOW_RESIZABLE;
    
    SDL_Window * window = nullptr;

    bool CreateWindow();
    void DestroyWindow();
};