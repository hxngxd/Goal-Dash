#include "game.h"
#include "inputhandler.h"

void KeyboardHandler::LeftPlayerInputHandler(Player & player){
    if (Game::event.type==SDL_KEYDOWN){
        switch (Game::event.key.keysym.sym){
            case SDLK_w:
                player.key_up = true;
                break;
            case SDLK_s:
                player.key_down = true;
                break;
            case SDLK_a:
                player.key_left = true;
                break;
            case SDLK_d:
                player.key_right = true;
                break;
            // case SDLK_SPACE:
            //     key_space = true;
            //     break;
            default:
                break;
        }
    }
    if (Game::event.type==SDL_KEYUP){
        switch (Game::event.key.keysym.sym){
            case SDLK_w:
                player.key_up = false;
                break;
            case SDLK_s:
                player.key_down = false;
                break;
            case SDLK_a:
                player.key_left = false;
                break;
            case SDLK_d:
                player.key_right = false;
                break;
            // case SDLK_SPACE:
            //     key_space = false;
            //     break;
            default:
                break;
        }
    }
}