#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "vector2.h"
#include "render.h"

struct Direction;

extern const char * title;
extern const Vector2 resolution;
extern const int map_size;
extern const int player_size;

extern float fps;
extern float player_speed;
extern float player_acceleration_rate;
extern float animation_speed;
extern float falling_speed;
extern float gravity;

extern std::vector<std::vector<int>> tileMap;

extern SDL_Window * window;
extern SDL_Renderer * renderer;

extern Sprite sprite_idle;
extern Sprite sprite_run;

struct Direction {
    float u, d, l, r;
    Direction() : u(0), d(0), l(0), r(0) {}
    Direction(float u, float d, float l, float r) : u(u), d(d), l(l), r(r) {}
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
    void Quit();

    bool isRunning();
};

class Player {
private:

public:

    enum animation {
        idle,
        run,
        push,
        pull,
        jump
    };
    enum direction {
        left,
        right
    };

    int frame = 0;
    const char * name;
    Vector2 position;
    Direction velocity;

    animation animation_state = idle;
    direction animation_direction = right;
    
    int state_frames = 0;
    float animation_delay = 0;

    void Init(const char * name, Vector2 position);

    ~Player();

    void Update();
    void Animation();
    void ScreenLimit();
    void Move();
};

class Map{
public:
    enum type{
        empty = 0,
        normal = 1,
        damage = 2,
    };
    int rows;
    int cols;
    Map(int rows, int cols, std::vector<float> tileProbabilities, std::vector<std::vector<int>> & tileMap);

    void Draw(std::vector<std::vector<int>> & tileMap);
};