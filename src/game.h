#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "vector2.h"

struct Direction;

extern const char * title;
extern const int width;
extern const int height;
extern const int map_size;

extern float fps;
extern float speed;

extern SDL_Window * window;
extern SDL_Renderer * renderer;

extern Direction velocity;

struct Direction {
    int u, d, l, r;
    Direction() : u(0), d(0), l(0), r(0) {}
    Direction(int u, int d, int l, int r) : u(u), d(d), l(l), r(r) {}
};

class Game {
private:
    bool running = false;

public:
    static SDL_Event event;

    void Start();
    void Init();
    void HandleEvent();
    void Update();
    void Render();
    void Quit();

    bool isRunning();
};