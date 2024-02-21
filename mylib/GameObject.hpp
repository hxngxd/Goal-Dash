#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "Math.hpp"

class GameObject{
public:
    GameObject(const char * name, Vector2 position, SDL_Rect src, const char * texturePath, SDL_Renderer * renderer);
    ~GameObject();

    const char * name;
    Vector2 postion;
    int scale = 1;
    
    SDL_Rect src;
    SDL_Rect dst;

    SDL_Texture * texture;
};