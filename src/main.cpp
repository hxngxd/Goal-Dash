#include <iostream>
#include "../include/SDL2/SDL.h"
#include "screen.h"
#include "game.h"

const char * Screen::title = "Fun game";
int Screen::width = 800;
int Screen::height = 800;
int Screen::scale = 16;

float Game::fps = 60.0;
int Game::speed = 5;

Game game;

int main(int argc, char* argv[]){
    
    game.Start();
    std::cout << game.isRunning();

    float frameStart, frameTime, frameDelay = 1000.0/Game::fps;

    while (game.isRunning()){

        frameStart = (float)SDL_GetTicks();

        game.HandleEvent();
        game.Update();
        game.Render();

        frameTime = (float)SDL_GetTicks() - frameStart;

        if (frameTime < frameDelay){
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game.Quit();

    return 0;
}