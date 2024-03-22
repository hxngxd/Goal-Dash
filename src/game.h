#pragma once
#include <bits/stdc++.h>

#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_mixer.h"
#include "../include/SDL2/SDL_ttf.h"
#include "datalib.h"
#include "gameobject.h"

class Screen;
class Renderer;
class Scene;

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

class EventHandler : public Game
{
  public:
    static void PlayerInputHandler(Player *player, Keys &keys);
    static void MouseInputHandler();
};

class Scene : public Game
{
  public:
    Scene();
    Scene(int map);
    void DeleteScene();
};

class DelayFunction
{
  public:
    int start_time;
    int delay_time;
    std::function<bool()> function;
    std::function<void()> post_function;
    static void Update();
    static Uint32 Create(
        int delay_time, std::function<bool()> function, std::function<void()> post_function = []() {});
    static void Remove(Uint32 DFid);
};

extern std::map<Uint32, DelayFunction *> DelayFunctions;

template <typename T> void print_(T msg)
{
    std::cout << msg << " ";
}

template <typename T, typename... Args> void print_(T msg, Args... args)
{
    std::cout << msg << " ";
    print_(args...);
}

template <typename... Args> void print(Args... args)
{
    print_(args...);
    std::cout << std::endl;
}