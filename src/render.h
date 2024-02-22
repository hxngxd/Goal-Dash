#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"

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
};

class Renderer{
private:

public:
    Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    SDL_Renderer * renderer = nullptr;

    bool CreateRenderer(SDL_Window * window);
    void DestroyRenderer();

    void Clear(SDL_Color color);
    void SetDrawColor(SDL_Color color);
    void PointGrid(SDL_Color color);
    void Display();
};

class TextureManager{
public:
    static SDL_Texture * LoadTexture(const char * name, const char * path, SDL_Renderer * renderer);
};