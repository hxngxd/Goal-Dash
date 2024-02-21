#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../mylib/Screen.hpp"
#include "../mylib/Renderer.hpp"
#include "../mylib/Game.hpp"

Game::Game(){}

Game::~Game(){}

bool Game::Init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "Error: SDL_IMAGE failed to initialize - " << SDL_GetError();
        return 0;
    }

    currentScreen.Init();
    if (currentScreen.window==nullptr) return 0;

    currentRenderer.Init(currentScreen.window);
    if (currentRenderer.renderer==nullptr) return 0;

    SDL_SetRenderDrawBlendMode(currentRenderer.renderer, SDL_BLENDMODE_BLEND);

    currentRenderer.SquareGrid.resize(currentScreen.scale, std::vector<bool>(currentScreen.scale, 0));

    gameObjects = {
        GameObject("Object1", Vector2(), TexRect.tex256, "texture/board.png", currentRenderer.renderer)
    };

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

}

void Game::Render(){
    currentRenderer.Clear(Color.black(255));
    currentRenderer.PointGrid(currentScreen, Color.white(127));
    currentRenderer.RenderGameObject(gameObjects[0]);
    currentRenderer.Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(currentRenderer.renderer);
    SDL_DestroyWindow(currentScreen.window);
    SDL_Quit();
}