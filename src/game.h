#pragma once
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_mixer.h"
#include "../include/SDL2/SDL_ttf.h"
#include "datalib/util.h"
#include "event/scene.h"
#include "gameobject/gameobject.h"
#include <map>

class Screen;

class Game
{
  private:
    bool running = false;

  public:
    static std::map<std::string, PropertiesType> Properties;

    static SDL_Event event;
    static SDL_Window *window;
    static SDL_Renderer *renderer;
    static Scene *scene;

    static Player *player;
    static int player_score;
    static bool player_won;
    static int player_time[3];
    static int player_health;

    void Start();
    void Stop()
    {
        running = false;
    }
    void Quit();

    bool LoadConfig();
    bool InitSDL2();
    bool LoadMedia();

    void Update();

    bool isRunning()
    {
        return running;
    }
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
