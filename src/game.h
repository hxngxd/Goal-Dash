#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_ttf.h"
#include "../include/SDL2/SDL_mixer.h"
#include "datalib.h"

class Screen;
class Renderer;

class Game {
private:
    bool running = false;

public:
    static float fps;
    static float player_move_speed;
    static float player_acceleration;
    static float animation_speed;
    static float jump_speed;
    static float gravity;

    static Vector2 startingPosition;
    static int player_score;
    static SDL_Event event;
    static SDL_Window * window;
    static SDL_Renderer * renderer;

    void Start();
    bool InitSDL2();
    bool LoadMedia();
    void Quit();

    void HandleEvent();
    void Update();

    bool isRunning();
};

class Screen : public Game{
public:
    static std::string title;
    static Vector2 resolution;
    static int map_size;
    static int player_size;

    static void Clear(SDL_Color color);
    static void SetDrawColor(SDL_Color color);
    static void PointGrid(SDL_Color color);
    static void Background();
    static void Display();
    static void DrawSprite(Sprite & sprite, Vector2 position, Vector2 size, int currentFrame, bool flip);

    static Vector2 bg_star_position, bg_star_position1, bg_cloud_position, bg_margin;
    static int bg_opacity;
    static bool bg_toggle;
};

void ShowMsg(int indent, msg_types type, std::string msg);