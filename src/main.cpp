#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../mylib/Game.hpp"

Game game;

int main(int argc, char* argv[]){
    
    game.Init();
    
    float frameStart, frameTime;

    while (game.isRunning()){

        frameStart = (float)SDL_GetTicks();

        game.HandleEvent();
        game.Update();
        game.Render();

        frameTime = (float)SDL_GetTicks() - frameStart;

        if (frameTime < Game::frameDelay){
            SDL_Delay(Game::frameDelay - frameTime);
        }
    }

    game.Quit();

    return 0;
}