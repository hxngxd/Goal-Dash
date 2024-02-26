#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "datalib.h"
#include "gameobject.h"

class Game {
private:
    bool running = false;

public:
    static float fps;
    static float player_moving_speed;
    static float player_acceleration_rate;
    static float animation_speed;
    static float jump_speed;
    static float gravity;

    static SDL_Event event;
    static SDL_Window * window;
    static SDL_Renderer * renderer;

    void Start();
    void Init();
    void HandleEvent();
    void Update();
    void Quit();
    bool isRunning();
};

class Renderer;

class Screen : public Game{
public:
    static const char * title;
    static Vector2 resolution;
    static int map_size;
    static int player_size;

    static const int x = SDL_WINDOWPOS_CENTERED;
    static const int y = SDL_WINDOWPOS_CENTERED;

    static const Uint32 fixedsize = 0;
    static const Uint32 resizable = SDL_WINDOW_RESIZABLE;

    static bool Init();
};

class Renderer : public Game{
public:
    static const Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    static void Clear(SDL_Color color);
    static void SetDrawColor(SDL_Color color);
    static void PointGrid(SDL_Color color);
    static void DrawSprite(Sprite & sprite, Vector2 position, Vector2 size, int currentFrame, bool flip);
    static void Display();

    static bool Init();
};