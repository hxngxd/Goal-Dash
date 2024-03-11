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
    float animation_speed;
    float wait_for_animation;
};

class Background : public GameObject{
public:
    int opacity;
    bool toggle;

    Background(
        std::string name,
        float scale
    );
    static bool loadBackground(
        std::string name,
        std::string path,
        int maxFrames,
        Vector2 realSize,
        float scale
    );

    void setOpacity();
    static void Move(
        Vector2 velocity,
        int index,
        float ratio
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
    static std::pair<float, Vector2> CreateTiles(int map);
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

    int score;

    void Init(
        std::string name
    );

    void Update();
    void Animation();
    void MoveRightLeft();
    void MoveDownUp();
    void Collision();
    void Jump();
    void DrawBox();

    bool key_right, key_left, key_down, key_up;

    void MapCollision(
        Vector2 nextTile,
        std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> & visit,
        std::queue<Vector2> & Q
    );

    std::pair<bool, bool> collide_down;
    std::pair<bool, bool> collide_up;

};