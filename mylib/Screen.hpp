#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

class Screen{
public:
    const char * title = "SDL2-Template";

    int width = 800;
    int height = 800;
    int scale = 16;

    int x = SDL_WINDOWPOS_CENTERED;
    int y = SDL_WINDOWPOS_CENTERED;

    Uint32 fixedsize = 0;
    Uint32 resizable = SDL_WINDOW_RESIZABLE;

    bool isFullscreen = false;
    bool isMaximized = false;
    bool isResizable = false;

    SDL_Window * window = nullptr;

    void Init();
};