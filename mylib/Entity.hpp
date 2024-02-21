#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

struct Vector2{
    float x;
    float y;
    
    Vector2() : x(0.0), y(0.0) {}
    Vector2(float x, float y) : x(x), y(y) {}

    friend std::ostream & operator << (std::ostream & out, Vector2 & v){
        out << "(" << v.x << ", " << v.y << ")\n";
    }
};

class Entity{
public:
    Entity(Vector2 position, SDL_Rect rect, float scale, SDL_Texture * texture);

    Vector2 position;
    float scale;
    SDL_Rect rect;
    SDL_Texture * texture;

};