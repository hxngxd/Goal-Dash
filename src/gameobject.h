#pragma once
#include "game.h"

struct Direction{
    float l, r, u, d;
    Direction () : l(0), r(0), u(0), d(0) {}
    Direction (float l, float r, float u, float d) : l(l), r(r), u(u), d(d) {}
};

class GameObject{
public:
    const char * name;
    Vector2 position;
    Vector2 size;
    Direction velocity;

    static bool isCollide(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2, int error);
};

class MapTile : public GameObject{
public:
    int type;
    MapTile(Vector2 position, Vector2 size, Direction velocity, int type);
    static std::vector<MapTile> Tiles;
    static void Create(std::vector<std::vector<int>> & map);
    static void Draw();
};

class Player : public GameObject{
public:
    enum animation_states{
        idle,
        run,
        jump
    };
    enum animation_directions{
        left,
        right
    };

    int currentFrame;
    int maxFrame;
    float animation_delay;
    animation_states current_state = idle;
    animation_states previous_state = idle;
    animation_directions direction = right;

    Sprite _idle, _run, _jump;

    void Init(const char * name, Vector2 position);
    void Update();
    void Animation();
    void Movement();
};