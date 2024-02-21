#pragma once
#include "../include/SDL2/SDL.h"
#include "../mylib/Screen.hpp"
#include "../mylib/Render.hpp"
#include "../mylib/Entity.hpp"

class Game {
private:

public:
    bool isRunning = false;

    Game();
    ~Game();

    bool Init();
    void HandleEvent();
    void Update();
    void Quit();

    Screen currentScreen;
    Render currentRenderer;
};