#include "game.h"

bool GameObject::inScale(GameObject * gameobj){
    if (gameobj->scale < 0.5){
        gameobj->scale += 0.1;
        return 0;
    }
    if (gameobj->scale < 0.75){
        gameobj->scale += 0.05;
        return 0;
    }
    if (gameobj->scale < 1){
        gameobj->scale += 0.025;
        return 0;
    }
    if (gameobj->scale > 1) gameobj->scale = 1;
    return 1;
}

bool GameObject::deScale(GameObject * gameobj){
    if (gameobj->scale > 0.5){
        gameobj->scale -= 0.1;
        return 0;
    }
    if (gameobj->scale > 0.25){
        gameobj->scale -= 0.05;
        return 0;
    }
    if (gameobj->scale > 0){
        gameobj->scale -= 0.025;
        return 0;
    }
    if (abs(gameobj->scale) <= 1e-5) gameobj->scale = 0;
    return 1;
}