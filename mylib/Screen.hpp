#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

class Screen{
private:

    bool isFullscreen = false;
    bool isMaximized = false;
    bool isResizable = false;

    SDL_Window * window = nullptr;

public:
    static constexpr const char * title = "Fun game";

    static constexpr int width = 800;
    static constexpr int height = 800;
    static constexpr int scale = 16;

    static constexpr int x = SDL_WINDOWPOS_CENTERED;
    static constexpr int y = SDL_WINDOWPOS_CENTERED;

    static constexpr Uint32 fixedsize = 0;
    static constexpr Uint32 resizable = SDL_WINDOW_RESIZABLE;

    void Init();
    SDL_Window * getWindow();
};