#include "game.h"

void GameObject::reScale(GameObject * gameobj, float to, float delay, float v, std::function<void()> post_function){
    if (abs(gameobj->scale - to) <= 0.001){
        gameobj->scale = to;
        return;
    }
    float * tmp_v = new float(v);
    bool increasing = to > gameobj->scale;
    DelayFunction::CreateDelayFunction(delay, std::bind([](GameObject * gameobj, float to, float * tmp_v, bool increasing){
        if (increasing && gameobj->scale < to){
            gameobj->scale += *tmp_v;
            *tmp_v /= 1.05;
            return 0;
        }
        if (!increasing && gameobj->scale > to){
            gameobj->scale -= *tmp_v;
            *tmp_v /= 1.05;
            return 0;
        }
        gameobj->scale = to;
        delete tmp_v;
        tmp_v = nullptr;
        return 1;
    }, gameobj, to, tmp_v, increasing), post_function);
}