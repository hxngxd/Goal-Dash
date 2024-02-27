#pragma once
#include "game.h"

class Player;

class GameObject{
public:
    const char * name;
    Vector2 position;
    Vector2 size;
    Direction velocity;

    int currentFrame;
    int maxFrame;
    float animation_delay;

    static void BFS_Collision(Player & player, Vector2 & playerCenter, Vector2 nextTile, std::vector<std::vector<bool>> & trace, std::queue<Vector2> & Q);
};

class MapTile : public GameObject{
public:
    static std::vector<MapTile> Tiles;

    enum tile_types{
        wall=1,
        coin
    };
    int type;
    Vector2 tile;
    MapTile(Vector2 position, Vector2 size, Direction velocity, int type, Vector2 tile);
    static void Create(std::vector<std::vector<int>> & map);
    static void Draw();
    static void Update();
};

class Player : public GameObject{
public:
    enum animation_states{
        idle=3,
        run,
        jump
    };
    enum animation_directions{
        left,
        right
    };

    animation_states current_state;
    animation_states previous_state;
    animation_directions direction;

    Player() = default;
    Player(const char * name, Vector2 position);
    void Update();
    void Animation();
    void Movement();

    bool key_right;
    bool key_left;
    bool key_down;
    bool key_up;

    bool collide_down;
    bool tmp_collide_down;
    bool collide_up;
    bool tmp_collide_up;
};