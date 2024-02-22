// #include <bits/stdc++.h>
// #include "../mylib/GameObject.hpp"
// #include "../mylib/Screen.hpp"
// #include "../mylib/Renderer.hpp"

// GameObject::GameObject(const char * name, Vector2 position, SDL_Rect src, const char * texturePath, SDL_Renderer * renderer){
//     this->name = name;
//     this->postion = position;
//     this->src = src;
//     this->dst = {(int)position.x, (int)position.y, (int)position.x+(Screen::width/Screen::scale), (int)position.y+(Screen::width/Screen::scale)};
//     this->texture = TextureManager::LoadTexture(name, texturePath, renderer);
//     std::cout << "Created gameobject: " << name << std::endl;
// }

// GameObject::~GameObject(){}

// void GameObject::setPosition(Vector2 position){
//     this->postion = position;
//     UpdatePosition();

// }

// void GameObject::UpdatePosition(){
//     this->dst.x = this->postion.x;
//     this->dst.y = this->postion.y;
// }

// Vector2 GameObject::getPosition(){
//     return this->postion;
// }

// SDL_Texture * GameObject::getTexture(){
//     return this->texture;
// }