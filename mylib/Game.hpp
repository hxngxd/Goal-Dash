#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../mylib/Screen.hpp"
#include "../mylib/Renderer.hpp"
#include "../mylib/GameObject.hpp"
#include "../mylib/Math.hpp"

class Game {
private:

public:
    bool isRunning = false;
    std::vector<GameObject> gameObjects;

    Game();
    ~Game();

    bool Init();
    void HandleEvent();
    void Update();
    void Render();
    void Quit();

    Screen currentScreen;
    Renderer currentRenderer;
};