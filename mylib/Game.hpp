#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../mylib/Screen.hpp"
#include "../mylib/Renderer.hpp"
#include "../mylib/GameObject.hpp"
#include "../mylib/Math.hpp"

class Game {
private:
    bool running = false;

public:
    std::vector<GameObject> gameObjects;

    const int speed = 3;
    static constexpr float FPS = 60.0;
    static constexpr float frameDelay = 1000.0 / FPS;

    Game();
    ~Game();

    void Init();
    void Start();
    void HandleEvent();
    void Update();
    void Render();
    void Quit();
    bool isRunning();

    Screen currentScreen;
    Renderer currentRenderer;
};