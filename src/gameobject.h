#pragma once
#include "datalib.h"

class Player;
class MapTile;
class Background;

extern std::vector<std::vector<int>> TileMap;
extern std::vector<MapTile> Tiles;
extern std::vector<Background> Backgrounds;

class GameObject{
public:
    std::string name;
    Vector2 starting_position, position;
    Vector2 size;
    float scale;
    Direction velocity;

    int currentFrame, maxFrames;
    float animation_delay;
    float wait_for_animation;
    float animation_speed;
    
    static void BFS_Collision(Player & player, Vector2 & playerCenter, Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> & visit, std::queue<Vector2> & Q, float maxDist, float eps);
};

class Background : public GameObject{
public:
    int opacity;
    bool toggle;
    Background(std::string name, float scale);
    void setOpacity();
    static bool loadBackground(
        std::string name,
        std::string path,
        int maxFrames,
        Vector2 realSize,
        float scale
    );
    static void Draw();
};

class MapTile : public GameObject{
public:
    int type;
    Vector2 index;
    MapTile(
        Vector2 position,
        Vector2 size,
        Vector2 index,
        int type,
        float wait
    );
    static int CreateTiles();
    static void CreateATile(
        int i,
        int j,
        float & wait
    );
    static void Draw();
    static void Update();
};

class Player : public GameObject{
public:
    Player() = default;

    animation_states current_state, previous_state;
    animation_directions direction;

    float move_speed;
    float acceleration;
    float jump_speed;

    void Init(std::string name, Vector2 position, int way);
    void Update();
    void Animation();
    void MoveRightLeft();
    void MoveDownUp();
    void Collision();
    void Jump();
    void DrawBox();
    void Damaged();

    bool key_right, key_left, key_down, key_up;

    std::pair<bool, bool> collide_down;
    std::pair<bool, bool> collide_up;
};