#pragma once
#include "datalib.h"

class Player;
class MapTile;

extern std::vector<std::vector<int>> tilemap;
extern std::vector<MapTile> Tiles;

class GameObject{
public:
    std::string name;
    Vector2 position;
    Vector2 size;
    Direction velocity;

    int currentFrame;
    int maxFrames;
    float animation_delay;

    Vector2 size_animation;
    float wait_for_animation;
    
    static void BFS_Collision(Player & player, Vector2 & playerCenter, Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> & visit, std::queue<Vector2> & Q, float maxDist, float eps);
};

class MapTile : public GameObject{
public:
    int type;
    Vector2 tile;

    MapTile(Vector2 position, Vector2 size, Direction velocity, int type, Vector2 tile, float wait_for_animation);
    static void Create();
    static void CreateATile(int i, int j, int & wait);
    static void Draw();
    static void Update();
};

class Player : public GameObject{
public:
    animation_states current_state;
    animation_states previous_state;
    animation_directions direction;

    Player() = default;

    void Init(std::string name, Vector2 position);
    void Update();
    void Animation();
    void MoveRightLeft();
    void MoveDownUp();
    void Collision();
    void Jump();
    void DrawBox();

    bool key_right;
    bool key_left;
    bool key_down;
    bool key_up;

    bool collide_down;
    bool tmp_collide_down;
    bool collide_up;
    bool tmp_collide_up;

    bool isMovingSound;
};