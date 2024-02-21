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

    void operator += (Vector2 v){
        x += v.x;
        y += v.y;
    }

    void operator -= (Vector2 v){
        x -= v.x;
        y -= v.y;
    }

    void operator *= (float k){
        x *= k;
        y *= k;
    }

    void operator /= (float k){
        x /= k;
        y /= k;
    }

    friend Vector2 operator + (Vector2 v1, Vector2 v2){
        return Vector2(v1.x + v2.x, v1.y + v2.y);
    }

    friend Vector2 operator - (Vector2 v1, Vector2 v2){
        return Vector2(v1.x - v2.x, v1.y - v2.y);
    }

    friend Vector2 operator * (Vector2 v, float k){
        return Vector2(v.x * k, v.y * k);
    }

    friend Vector2 operator / (Vector2 v, float k){
        return Vector2(v.x / k, v.y / k);
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