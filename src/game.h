#pragma once
#include <iostream>
#include "../include/SDL2/SDL.h"
#include "screen.h"
#include "render.h"

class Game {
private:
    bool running = false;

public:
    static int speed;

    static float fps;

    static SDL_Event event;

    void Start();
    void Init();
    void HandleEvent();
    void Update();
    void Render();
    void Quit();

    bool isRunning();

    static Screen currentScreen;
    static Renderer currentRenderer;
};