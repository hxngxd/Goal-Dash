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

    int currentFrame, maxFrames;
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

class MapTile : public GameObject
{
  public:
    static Vector2 SpawnTile;
    static Vector2 WinTile;
    static bool isMakingMap;
    static int nEmptyTiles;

    MapTile(Vector2 position, Vector2 size);

    static void CreateBorder();
    static void CreateTiles(int map);
    static void CreateATile(int i, int j, float &wait);
    static void DeleteTiles();
    static void DeleteATile(int i, int j, float &wait);
    static void Update();
};

class RandomMap
{
  public:
    static void Random();
    static bool Validation(int ei, int ej);
    static void EmptyToEmpty(int i, int j, std::vector<std::vector<bool>> &visit);
};

extern std::vector<std::vector<std::pair<int, MapTile *>>> TileMap;

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
    bool isDamaged[3];
};

void MapHUD();