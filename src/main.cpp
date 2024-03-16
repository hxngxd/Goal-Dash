#include "game.h"

int main(int argc, char *argv[])
{
    //----------------------------------------

    ShowMsg(0, normal, "starting new game...");
    Game *game = new Game();
    game->Start();

    //----------------------------------------

    if (game->isRunning())
        ShowMsg(0, success, "game is running!");

    float frameStart, frameTime, frameDelay = 1000.0 / Game::Properties["fps"].f;

    while (game->isRunning())
    {

        frameStart = (float)SDL_GetTicks();

        game->HandleEvent();
        game->Update();

        frameTime = (float)SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay)
            SDL_Delay(frameDelay - frameTime);
    }

    //----------------------------------------

    ShowMsg(0, normal, "trying to quit game...");
    game->Quit();
    delete game;
    game = nullptr;
    ShowMsg(0, success, "game closed.");
    return 0;
}