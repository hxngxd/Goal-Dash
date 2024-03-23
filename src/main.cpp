#include "datalib/msg.h"
#include "game.h"

Game *game = nullptr;

int main(int argc, char *argv[])
{
    //----------------------------------------

    print("starting new game...");
    game = new Game();
    game->Start();

    //----------------------------------------

    if (game->isRunning())
        print("game in running");

    float frameStart, frameTime, frameDelay = 1000.0 / Game::Properties["fps"].f;

    while (game->isRunning())
    {

        frameStart = (float)SDL_GetTicks();

        game->Update();

        frameTime = (float)SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay)
            SDL_Delay(frameDelay - frameTime);
    }

    //----------------------------------------

    print("closing game...");
    game->Quit();
    delete game;
    game = nullptr;
    print("game closed");
    return 0;
}