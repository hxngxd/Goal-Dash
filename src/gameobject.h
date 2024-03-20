#pragma once
#include "datalib.h"

class Player;
class MapTile;
class Background;
class UI;
class Button;

extern std::vector<std::vector<std::pair<int, MapTile *>>> TileMap;
extern std::vector<Background> Backgrounds;
extern std::map<std::string, Button *> Buttons;
extern std::string hoverButton, downButton, upButton;

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
    static void Draw();
};

class MapTile : public GameObject
{
  public:
    static Vector2 SpawnTile;
    static Vector2 WinTile;

    MapTile(Vector2 position, Vector2 size, float wait);

    static void CreateBorder();
    static float CreateTiles(int map);
    static void CreateATile(int i, int j, float &wait);
    static float DeleteTiles();
    static void Draw();
};

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

class UI
{
  public:
    std::string name;
    Vector2 position;
    float scale;
    float bg_opacity;
    Uint32 DFid[3] = {0, 0, 0};

    static void Update();
    static void DeleteUIs();
};

class Button : public UI
{
  public:
    std::string label;
    float font_size;
    std::function<void()> onClick;

    static bool CreateButton(
        std::string name, const Vector2 &position, std::string label, std::function<void()> onClick = []() {});

    void Update();
};