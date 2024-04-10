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

    if (game->running)
        print("game is running");

    float frame_start, frame_time, frame_delay = 1000.0 / Game::properties["fps"].f;

    while (game->running)
    {
        frame_start = (float)SDL_GetTicks();

        game->Update();

        frame_time = (float)SDL_GetTicks() - frame_start;
        if (frame_time < frame_delay)
            SDL_Delay(frame_delay - frame_time);
    }

    //----------------------------------------

    print("closing game...");
    game->Quit();
    delete game;
    game = nullptr;
    print("game closed");
    return 0;
}