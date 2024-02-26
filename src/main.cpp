#include "game.h"

Game * game;

int main(int argc, char* argv[]){

    game = new Game();
    game->Start();

    if (game->isRunning()) std::cout << "Game is running!" << std::endl;

    float frameStart, frameTime, frameDelay = 1000.0/Game::fps;

    while (game->isRunning()){

        frameStart = (float)SDL_GetTicks();

        game->HandleEvent();
        game->Update();

        frameTime = (float)SDL_GetTicks() - frameStart;

        if (frameTime < frameDelay){
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game->Quit();
    delete game;
    game = nullptr;

    return 0;
}