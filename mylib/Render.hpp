#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "Screen.hpp"
#include "Entity.hpp"

class Render{
public:
    //renderer flags
    Uint32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    SDL_Renderer * renderer = NULL;

    //method
    Render(SDL_Window * window);
    SDL_Texture * loadTexture(const char * path);
    void Clear();
    void RenderIt(Entity & entity);
    void Display();
    void CleanUp();
};