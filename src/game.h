#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_ttf.h"
#include "../include/SDL2/SDL_mixer.h"
#include "datalib.h"
#include "gameobject.h"

class Screen;
class Renderer;

class Game {
private:
    bool running = false;

public:
    static std::map<std::string, PropertiesType> Properties;

    static SDL_Event event;
    static SDL_Window * window;
    static SDL_Renderer * renderer;

    void Start();
    void Quit();

    bool LoadConfig();
    bool InitSDL2();
    bool LoadMedia();

    void HandleEvent();
    void Update();

    bool isRunning();
};

class Screen : public Game{
public:
    static Vector2 resolution;
    static int map_size;
    static int tile_size;

    static void Clear(
        SDL_Color color
    );
    static void SetDrawColor(
        SDL_Color color
    );
    static void PointGrid(
        SDL_Color color
    );
    static void Display();
    static void DrawSprite(
        Sprite & sprite,
        const Vector2 & position,
        const Vector2 & size,
        float scale,
        int currentFrame,
        bool flip
    );
};

class KeyboardHandler : public Game{
public:
    static void PlayerInputHandler(
        Player & player, 
        Keys & keys
    );
};

class Hub : public Game{

};

class DelayFunction{
public:
    int start_time;
    int delay_time;
    std::function<void()> function;
    void static Update();
    void static CreateDelayFunction(
        int delay_time,
        std::function<void()> function
    );
};

extern std::map<int, DelayFunction*> DelayFunctions;

template <class T>
void ShowMsg(
    int indent,
    msg_types type,
    T msg
){
    if (!Game::Properties["show_msg"].b) return;
    std::cout << std::string(indent * 2, ' ');
    switch (type){
        case normal:
            std::cout << "> ";
            break;
        case success:
            std::cout << "$ ";
            break;
        case fail:
            std::cout << "@ ";
            break;
        case error:
            std::cout << "! ";
            break;
        case logging:
            std::cout << "# ";
            break;
        default:
            break;
    }
    std::cout << msg << std::endl;
}