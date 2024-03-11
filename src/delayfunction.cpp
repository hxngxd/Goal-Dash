#include "game.h"

void DelayFunction::CreateDelayFunction(
    int delay_time,
    std::function<void()> function
){
    int currentTime = SDL_GetTicks();
    DelayFunctions[currentTime + delay_time] = new DelayFunction();
    DelayFunctions[currentTime + delay_time]->start_time = currentTime;
    DelayFunctions[currentTime + delay_time]->delay_time = delay_time;
    DelayFunctions[currentTime + delay_time]->function = function;
}

void DelayFunction::Update(){
    if (DelayFunctions.empty()) return;
    std::vector<int> tasks;
    for (auto & func : DelayFunctions){
        if (SDL_GetTicks() - func.second->start_time >= func.second->delay_time){
            tasks.push_back(func.first);
            func.second->function();
        }
    }
    if (tasks.empty()) return;
    for (auto t : tasks){
        delete DelayFunctions[t];
        DelayFunctions[t] = nullptr;
        DelayFunctions.erase(t);
    }
}