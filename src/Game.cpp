#include <iostream>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "game.h"
#include "screen.h"
#include "render.h"

SDL_Event Game::event;
Screen Game::currentScreen;
Renderer Game::currentRenderer;

void Game::Start(){
    Init();
}

void Game::Init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "Error: SDL_IMAGE failed to initialize - " << SDL_GetError();
        return;
    }

    if (!currentScreen.CreateWindow()) return;

    if (!currentRenderer.CreateRenderer(currentScreen.window)) return;

    SDL_SetRenderDrawBlendMode(currentRenderer.renderer, SDL_BLENDMODE_BLEND);

    running = true;
}

bool Game::isRunning(){
    return running;
}

void Game::HandleEvent(){
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void Game::Update() {

}

void Game::Render(){
    currentRenderer.Clear(Color::black(255));
    currentRenderer.PointGrid(Color::white(127));
    currentRenderer.Display();
}

void Game::Quit(){
    currentRenderer.DestroyRenderer();
    currentScreen.DestroyWindow();
    SDL_Quit();
}