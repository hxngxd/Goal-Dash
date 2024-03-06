#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

struct Vector2{
    float x;
    float y;
    
    Vector2() : x(0.0), y(0.0) {}
    Vector2(float value) : x(value), y(value) {} 
    Vector2(float x, float y) : x(x), y(y) {}

    static const Vector2 right, left, up, down;

    static Vector2 Int(Vector2 v);

    static bool IsInRange(Vector2 & v, float minX, float maxX, float minY, float maxY);
    
    friend std::ostream & operator << (std::ostream & out, const Vector2 & v){
        out << "(" << v.x << ", " << v.y << ")";
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

    static Vector2 max(Vector2 v1, Vector2 v2){
        return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
    }

    static Vector2 min(Vector2 v1, Vector2 v2){
        return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
    }

    Vector2 operator + (Vector2 other){
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator - (Vector2 other){
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator * (float k){
        return Vector2(x * k, y * k);
    }

    Vector2 operator / (float k){
        return Vector2(x / k, y / k);
    }

    float distance(Vector2 other);
};

struct Vector2Hash {
    std::size_t operator()(const Vector2& v) const {
        std::hash<float> hash_fn;
        return hash_fn(v.x) ^ hash_fn(v.y);
    }
};

struct Vector2Equal {
    bool operator()(const Vector2& lhs, const Vector2& rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

struct Direction{
    float l, r, u, d;
    Direction () : l(0), r(0), u(0), d(0) {}
    Direction (float l, float r, float u, float d) : l(l), r(r), u(u), d(d) {}
};

struct Color{
    static SDL_Color white  (Uint8 a) { return {255, 255, 255, a}; }
    static SDL_Color black  (Uint8 a) { return {  0,   0,   0, a}; }
    static SDL_Color blue   (Uint8 a) { return {  0,   0, 255, a}; }
    static SDL_Color red    (Uint8 a) { return {255,   0,   0, a}; }
    static SDL_Color green  (Uint8 a) { return {  0, 255,   0, a}; }
    static SDL_Color violet (Uint8 a) { return {238, 130, 238, a}; }
    static SDL_Color pink   (Uint8 a) { return {255, 192, 203, a}; }
    static SDL_Color yellow (Uint8 a) { return {255, 255,   0, a}; }
    static SDL_Color orange (Uint8 a) { return {255, 165,   0, a}; }
    static SDL_Color cyan   (Uint8 a) { return {  0, 255, 255, a}; }
};

struct Rect{
    static SDL_Rect Square (int res) { return {0, 0, res, res}; }
    static bool isCollide(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2);
    static Vector2 RectCenter(Vector2 position, Vector2 size);
};

class Sprite{
public:
    SDL_Texture * texture;
    std::string path;
    int maxFrames;
    Vector2 realSize;
    
    Sprite() = default;
    Sprite(std::string path, int maxFrames, Vector2 realSize);
};

struct AllSprite{
    Sprite wall, coin, idle, run, jump;
    void LoadAllSprite();
};