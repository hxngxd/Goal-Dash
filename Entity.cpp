#include <bits/stdc++.h>
#include "mylib/Entity.hpp"

Entity::Entity(Position position, SDL_Rect rect, SDL_Texture * texture){
    this->position = position;
    this->rect = rect;
    this->texture = texture;
}