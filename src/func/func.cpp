#include "func.h"
#include "../datalib/util.h"
#include <vector>

std::map<Uint32, LinkedFunctions *> Functions;

FunctionNode::FunctionNode(std::function<bool()> func, Uint32 delay_time, Uint32 times)
{
    this->start_time = SDL_GetTicks();
    this->delay_time = delay_time;
    this->times = times;
    this->func = func;
    this->next = nullptr;
}

LinkedFunctions::LinkedFunctions(FunctionNode *firstFunc)
{
    first = last = nullptr;
    id = RandUint32.rand();
    while (Functions.find(id) != Functions.end() || !id)
        id = RandUint32.rand();
    Functions[id] = this;
    NextFunction(firstFunc);
}

void LinkedFunctions::NextFunction(FunctionNode *nextFunc)
{
    if (!first)
        first = last = nextFunc;
    else
    {
        last->next = nextFunc;
        last = last->next;
    }
}

void LinkedFunctions::Update()
{
    if (Functions.empty())
        return;

    std::vector<Uint32> ids;

    for (auto &func : Functions)
    {
        if (!func.second || !func.second->first)
        {
            ids.push_back(func.first);
            continue;
        }

        if (SDL_GetTicks() - func.second->first->start_time >= func.second->first->delay_time)
        {
            if (func.second->first->times)
            {
                if (func.second->first->func())
                    func.second->first->times--;
            }
            else
            {
                FunctionNode *temp = func.second->first;
                func.second->first = func.second->first->next;
                delete temp;
                temp = nullptr;
                if (!func.second->first)
                    func.second->last = nullptr;
                else
                    func.second->first->start_time = SDL_GetTicks();
            }
        }
    }

    if (ids.empty())
        return;

    for (auto id : ids)
        Remove(id);
}

void LinkedFunctions::Remove(Uint32 id)
{
    if (!id)
        return;
    if (Functions.find(id) == Functions.end())
        return;
    LinkedFunctions *&FN = Functions[id];
    if (FN)
    {
        delete FN;
        FN = nullptr;
    }
    Functions.erase(id);
}