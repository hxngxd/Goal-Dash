#pragma once
#include "../../include/SDL2/SDL.h"
#include <functional>
#include <map>

class FunctionNode
{
  public:
    Uint32 start_time;
    Uint32 delay_time;
    Uint32 times;

    std::function<bool()> func;
    FunctionNode *next;

    FunctionNode(std::function<bool()> func, Uint32 delay_time = 0, Uint32 times = 1);
};

class LinkedFunctions
{
  public:
    FunctionNode *first;
    FunctionNode *last;
    Uint32 id;

    LinkedFunctions(FunctionNode *firstFunc);
    void NextFunction(FunctionNode *nextFunc);
    static void Remove(Uint32 id);

    static void Update();
};

extern std::map<Uint32, LinkedFunctions *> Functions;