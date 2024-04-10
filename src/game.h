#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_mixer.h"
#include "../include/SDL2/SDL_ttf.h"
#include "datalib/util.h"
#include "event/scene.h"
#include "gameobject/gameobject.h"
#include "gameobject/map.h"
#include <map>

class Screen;

class Game
{
  public:
    static bool running;
    static std::map<std::string, PropertiesType> properties;
    static SDL_Event event;
    static SDL_Window *window;
    static SDL_Renderer *renderer;
    static Player *player;
    static Scene *scene;
    static int time[3];

    void Start();
    void Quit();
    bool LoadConfig();
    bool InitSDL2();
    bool LoadMedia();
    void Update();
};

extern Game *game;

class Screen : public Game
{
  public:
    static Vector2 resolution;
    static int map_size;
    static int tile_size;

    static void Clear(SDL_Color color);
    static void SetDrawColor(SDL_Color color);
    static void PointGrid(SDL_Color color);
    static void Display();
};
