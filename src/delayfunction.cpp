#include "game.h"

void DelayFunction::Create(int delay_time, std::function<bool()> function, std::function<void()> post_function)
{
    int currentTime = SDL_GetTicks();
    float id = currentTime + delay_time;
    while (DelayFunctions.find(id) != DelayFunctions.end())
        id += 0.01;

    DelayFunctions[id] = new DelayFunction();
    DelayFunctions[id]->start_time = currentTime;
    DelayFunctions[id]->delay_time = delay_time;
    DelayFunctions[id]->function = function;
    DelayFunctions[id]->post_function = post_function;
}

void DelayFunction::Update()
{
    if (DelayFunctions.empty())
        return;

    std::vector<int> tasks;
    for (auto &func : DelayFunctions)
    {
        if (!func.second)
            continue;
        if (SDL_GetTicks() - func.second->start_time >= func.second->delay_time)
        {
            if (func.second->function())
            {
                func.second->post_function();
                tasks.push_back(func.first);
            }
        }
    }

    if (tasks.empty())
        return;

    for (auto t : tasks)
    {
        delete DelayFunctions[t];
        DelayFunctions[t] = nullptr;
        DelayFunctions.erase(t);
    }
}