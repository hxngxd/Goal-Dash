#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "vector2.h"
#include "render.h"
#include "gameobject.h"
#include "screen.h"
#include "keyboard.h"

extern const char * title;
extern const Vector2 resolution;
extern const int map_size;
extern const int player_size;

extern float fps;
extern float player_moving_speed;
extern float player_acceleration_rate;
extern float animation_speed;
extern float jump_speed;
extern float gravity;

extern SDL_Window * window;
extern SDL_Renderer * renderer;

class Game {
private:
    bool running = false;
public:
    static SDL_Event event;
    void Start();
    void Init();
    void HandleEvent();
    void Update();
    void Quit();
    bool isRunning();
};