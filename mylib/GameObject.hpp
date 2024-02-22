#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "Math.hpp"

class GameObject{
private:
    const char * name;

    Vector2 postion;

    SDL_Texture * texture;

    void UpdatePosition();

public:
    GameObject(const char * name, Vector2 position, SDL_Rect src, const char * texturePath, SDL_Renderer * renderer);
    ~GameObject();
    
    Vector2 getPosition();
    void setPosition(Vector2 position);

    void MoveUp(int step);
    void MoveDown(int step);
    void MoveRight(int step);
    void MoveLeft(int step);

    void MoveUpRight(int step);
    void MoveUpLeft(int step);
    void MoveDownRight(int step);
    void MoveDownLeft(int step);

    SDL_Rect src;
    SDL_Rect dst;

    SDL_Texture * getTexture();
};