#pragma once
#include "gameobject.h"

class Tile;

class Map
{
  public:
    static std::vector<std::vector<std::pair<int, Tile *>>> Tiles;
    static Vector2 spawn_tile, win_tile;
    static bool mode;
    static int nempty;
    static int current_map;
    static std::map<int, int> count_types;

    static void Border();
    static void LoadMap();
    static void AddTile(int i, int j, float &wait, bool animation = true);
    static void AddTiles();
    static void RemoveTile(int i, int j, float &wait, bool animation = true);
    static void RemoveTiles();
    static void Update();
};

class Tile : public GameObject
{
  public:
    Tile(Vector2 position, Vector2 size);
};

class MapMaking
{
  public:
    static Vector2 current_mouse_tile;
    static int current_drawing_type;
    static bool allow_drawing;

    static void Random();
    static bool Validation(int ei, int ej);
    static void EmptyToEmpty(int i, int j, std::vector<std::vector<bool>> &visit);
    static void Trajectory(int i, int j, float u, float v, bool isRight, std::vector<std::vector<bool>> &visitable);
    static void Horizontal(int i, int j, bool isRight, std::vector<std::vector<bool>> &visitable);
    static void DownVertical(int i, int j, std::vector<std::vector<bool>> &visitable);

    static void Clear(LinkedFunction *post_func);
    static void Save();
    static void ChangeMap();

    static void Update();
};