#include <bits/stdc++.h>
#include "../mylib/Entity.hpp"

Entity::Entity(Vector2 position, SDL_Rect rect, float scale, SDL_Texture * texture){
    this->position = position;
    this->rect = rect;
    this->scale = scale;
    this->texture = texture;
}