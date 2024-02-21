#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_mixer.h"
#include "../include/SDL2/SDL_ttf.h"
#include "../mylib/Game.hpp"

Game game;

int main(int argc, char* argv[]){
    
    game.isRunning = game.Init();
    
    while (game.isRunning){
        game.HandleEvent();
        game.Update();
    }

    game.Quit();
    return 0;
}