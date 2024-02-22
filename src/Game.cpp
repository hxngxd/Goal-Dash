#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../mylib/Screen.hpp"
#include "../mylib/Renderer.hpp"
#include "../mylib/Game.hpp"

Game::Game(){}

Game::~Game(){}

void Game::Init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "Error: SDL_IMAGE failed to initialize - " << SDL_GetError();
        return;
    }

    currentScreen.Init();
    if (currentScreen.getWindow()==nullptr) return;

    currentRenderer.Init(currentScreen.getWindow());
    if (currentRenderer.getRenderer()==nullptr) return;

    SDL_SetRenderDrawBlendMode(currentRenderer.getRenderer(), SDL_BLENDMODE_BLEND);

    running = true;

    Start();
}

void Game::Start(){
    currentRenderer.SquareGrid.resize(Screen::scale, std::vector<bool>(Screen::scale, 0));

    gameObjects = {
        GameObject("Object1", Vector2(), TexRect.Square(256), "texture/board.png", currentRenderer.getRenderer()),
        GameObject("Object2", Vector2(), TexRect.Square(256), "texture/board.png", currentRenderer.getRenderer()),
        GameObject("Object3", Vector2(), TexRect.Square(256), "texture/board.png", currentRenderer.getRenderer())
    };
}

bool Game::isRunning(){
    return running;
}

void Game::HandleEvent(){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void Game::Update(){
    gameObjects[0].MoveRight(Game::speed);
    gameObjects[1].MoveDown(Game::speed);
    gameObjects[2].MoveDownRight(Game::speed);
    gameObjects[2].MoveRight(Game::speed);
}

void Game::Render(){
    currentRenderer.Clear(Color.black(255));
    currentRenderer.PointGrid(Color.white(127));

    for (GameObject gameObject : gameObjects){
        currentRenderer.RenderGameObject(gameObject);
    }

    currentRenderer.Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(currentRenderer.getRenderer());
    SDL_DestroyWindow(currentScreen.getWindow());
    SDL_Quit();
}