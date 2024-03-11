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
    static float fps;
    static float gravity;

    static SDL_Event event;
    static SDL_Window * window;
    static SDL_Renderer * renderer;

    void Start();
    void Quit();

    bool InitSDL2();
    bool LoadMedia();

    void HandleEvent();
    void Update();

    bool isRunning();
};

class Screen : public Game{
public:
    static std::string title;
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

template <class T>
void ShowMsg(
    int indent,
    msg_types type,
    T msg
){
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