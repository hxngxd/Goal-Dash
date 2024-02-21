#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../mylib/Math.hpp"

class Entity{
public:
    Entity(Vector2 position, SDL_Rect rect, float scale, SDL_Texture * texture);
    Vector2 position;
    float scale;
    SDL_Rect rect;
    SDL_Texture * texture;
};