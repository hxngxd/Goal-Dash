#include "func.h"
#include "../datalib/msg.h"
#include "../datalib/util.h"
#include <vector>

std::map<Uint32, LinkedFunction *> Functions;

FunctionNode::FunctionNode(std::function<bool()> func, Uint32 delay_time, Uint32 times)
{
    this->start_time = SDL_GetTicks();
    this->delay_time = delay_time;
    this->times = times;
    this->func = func;
    this->next = nullptr;
}

LinkedFunction::LinkedFunction(std::function<bool()> func, Uint32 delay_time, Uint32 times)
{
    first = last = nullptr;
    NextFunction(func, delay_time, times);
}

LinkedFunction::LinkedFunction(FunctionNode *firstFunc)
{
    first = last = nullptr;
    if (!firstFunc)
    {
        delete this;
        return;
    }
    NextFunction(firstFunc);
}

void LinkedFunction::NextFunction(std::function<bool()> func, Uint32 delay_time, Uint32 times)
{
    FunctionNode *newFunc = new FunctionNode(func, delay_time, times);
    NextFunction(newFunc);
}

void LinkedFunction::NextFunction(FunctionNode *nextFunc)
{
    if (!nextFunc)
        return;

    if (!first)
        first = last = nextFunc;
    else
    {
        last->next = nextFunc;
        last = last->next;
    }
    this->size++;
}

void LinkedFunction::Execute()
{
    id = RandUint32.rand();
    while (Functions.find(id) != Functions.end() || !id)
        id = RandUint32.rand();
    Functions[id] = this;
}

void LinkedFunction::Update()
{
    if (Functions.empty())
        return;

    std::vector<Uint32> ids;

    for (auto &func : Functions)
    {
        if (!func.second || !func.second->first || !func.second->size)
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
                func.second->size--;
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

void LinkedFunction::Remove(Uint32 id)
{
    if (!id)
        return;
    if (Functions.find(id) == Functions.end())
        return;
    LinkedFunction *&FN = Functions[id];
    if (FN)
    {
        delete FN;
        FN = nullptr;
    }
    Functions.erase(id);
}