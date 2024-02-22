#include <bits/stdc++.h>
#include "../mylib/GameObject.hpp"
#include "../mylib/Screen.hpp"
#include "../mylib/Renderer.hpp"

GameObject::GameObject(const char * name, Vector2 position, SDL_Rect src, const char * texturePath, SDL_Renderer * renderer){
    this->name = name;
    this->postion = position;
    this->src = src;
    this->dst = {position.x, position.y, position.x+(Screen::width/Screen::scale), position.y+(Screen::width/Screen::scale)};
    this->texture = TextureManager::LoadTexture(name, texturePath, renderer);
    std::cout << "Created gameobject: " << name << std::endl;
}

GameObject::~GameObject(){}

void GameObject::setPosition(Vector2 position){
    this->postion = position;
    UpdatePosition();

}

void GameObject::UpdatePosition(){
    this->dst.x = this->postion.x;
    this->dst.y = this->postion.y;
}

Vector2 GameObject::getPosition(){
    return this->postion;
}

void GameObject::MoveUp(int step){
    this->postion.y -= step;
    UpdatePosition();
}

void GameObject::MoveDown(int step){
    this->postion.y += step;
    UpdatePosition();
}

void GameObject::MoveRight(int step){
    this->postion.x += step;
    UpdatePosition();
}

void GameObject::MoveLeft(int step){
    this->postion.y -= step;
    UpdatePosition();
}

void GameObject::MoveUpRight(int step){
    step = (int)(std::cos(M_PI/4) * step);
    MoveUp(step);
    MoveRight(step);
}

void GameObject::MoveUpLeft(int step){
    step = (int)(std::cos(M_PI/4) * step);
    MoveUp(step);
    MoveLeft(step);
}

void GameObject::MoveDownRight(int step){
    step = (int)(std::cos(M_PI/4) * step);
    MoveDown(step);
    MoveRight(step);
}

void GameObject::MoveDownLeft(int step){
    step = (int)(std::cos(M_PI/4) * step);
    MoveDown(step);
    MoveLeft(step);
}

SDL_Texture * GameObject::getTexture(){
    return this->texture;
}