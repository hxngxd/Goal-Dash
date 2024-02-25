#include "gameobject.h"

bool GameObject::isCollide(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2){
    float c_x = abs(pos1.x - pos2.x) - size1.x/2 - size2.x/2;
    float c_y = abs(pos1.y - pos2.y) - size1.y/2 - size2.y/2;
    return c_x <= 0 && c_y <= 0;
}