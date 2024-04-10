#pragma once
#include "../datalib/util.h"
#include "../datalib/vector2.h"
#include <map>
#include <queue>
#include <unordered_map>

class Player;

class GameObject
{
  public:
    std::string name;
    Vector2 position;
    Vector2 size;
    float scale;
    Direction velocity;

    int current_frame, max_frames;
    float animation_delay;
    float animation_speed;
};

class Background : public GameObject
{
  public:
    int opacity;
    bool toggle;

    Background(std::string name, float scale);
    static bool loadBackground(std::string name, std::string path, int maxFrames, Vector2 realSize, float scale);

    static void Move(Vector2 velocity, int index, float ratio);
    static void Update();
};

extern std::vector<Background> Backgrounds;

class Player : public GameObject
{
  public:
    Player() = default;

    animation_states current_state, previous_state;
    animation_directions direction;

    Player(Vector2 position);
    ~Player();

    void Update();
    void Animation();
    void MoveRightLeft();
    void MoveDownUp();
    void Collision();
    void Jump();
    void DrawBox();
    void Damaged(bool c);

    bool key_right, key_left, key_down, key_up;

    void MapCollision(Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> &visit,
                      std::queue<Vector2> &Q);

    std::pair<bool, bool> collide_down;
    std::pair<bool, bool> collide_up;
};