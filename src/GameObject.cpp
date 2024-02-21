#include <bits/stdc++.h>
#include "../mylib/GameObject.hpp"
#include "../mylib/Renderer.hpp"

GameObject::GameObject(const char * name, Vector2 position, SDL_Rect src, const char * texturePath, SDL_Renderer * renderer){
    this->name = name;
    this->postion = position;
    this->src = src;
    this->dst = {position.x, position.y, position.x+src.w*this->scale, position.y+src.h*this->scale};
    this->texture = TextureManager::LoadTexture(name, texturePath, renderer);
}

GameObject::~GameObject(){}