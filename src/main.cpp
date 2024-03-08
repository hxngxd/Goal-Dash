#include "game.h"

int main(int argc, char* argv[]){

    ShowMsg(0, normal, "starting new game...");
    Game * game = new Game();
    game->Start();

    if (game->isRunning()) ShowMsg(0, success, "game is running!");

    float frameStart, frameTime, frameDelay = 1000.0/game->fps;

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
    ShowMsg(0, normal, "quit game.");
    return 0;
}