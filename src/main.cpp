#include "game.h"

Game game;

int main(int argc, char* argv[]){

    game.Start();
    if (game.isRunning()) std::cout << "RUNNING!" << std::endl;

    float frameStart, frameTime, frameDelay = 1000.0/fps;

    while (game.isRunning()){

        frameStart = (float)SDL_GetTicks();

        game.HandleEvent();
        game.Update();

        frameTime = (float)SDL_GetTicks() - frameStart;

        if (frameTime < frameDelay){
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game.Quit();

    return 0;
}