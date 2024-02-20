#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

struct Position{
    double x;
    double y;
};

class Entity{
public:
    Entity(Position position, SDL_Rect rect, SDL_Texture * texture);

    Position position;
    SDL_Rect rect;
    SDL_Texture * texture;

};