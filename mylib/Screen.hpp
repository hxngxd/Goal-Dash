#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

class Screen{
public:
    //title
    const char * title = "SDL2-Template";

    //resolution
    int width = 1280;
    int height = 720;

    //position
    int x = SDL_WINDOWPOS_CENTERED;
    int y = SDL_WINDOWPOS_CENTERED;

    //window flags
    Uint32 fixedsize = 0;
    Uint32 resizable = SDL_WINDOW_RESIZABLE;

    //other properties
    bool isFullscreen = false;
    bool isMaximized = false;
    bool isResizable = false;

    SDL_Window * window = NULL;

    //method
    Screen();
    void CleanUp();
};