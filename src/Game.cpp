#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../mylib/Screen.hpp"
#include "../mylib/Render.hpp"
#include "../mylib/Entity.hpp"
#include "../mylib/Game.hpp"

Game::Game(){}

Game::~Game(){}

bool Game::Init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return 0;
    }

    currentScreen.Init();
    if (currentScreen.window==nullptr) return 0;

    currentRenderer.Init(currentScreen.window);
    if (currentRenderer.renderer==nullptr) return 0;

    SDL_SetRenderDrawBlendMode(currentRenderer.renderer, SDL_BLENDMODE_BLEND);

    currentRenderer.SquareGrid.resize(currentScreen.scale, std::vector<bool>(currentScreen.scale, 0));
    return 1;
}

void Game::HandleEvent(){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::Update(){
    currentRenderer.Clear(Color.black(255));
    currentRenderer.PointGrid(currentScreen, Color.white(127));
    currentRenderer.Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(currentRenderer.renderer);
    SDL_DestroyWindow(currentScreen.window);
    SDL_Quit();
}