#include "game.h"
#include "keyboard.h"
#include <typeinfo>

void KeyboardHandler::PlayerMovement(Player & player){
    if (Game::event.type==SDL_KEYDOWN){
        switch (Game::event.key.keysym.sym){
            case SDLK_w:
                player.velocity.u = -1;
                break;
            case SDLK_a:
                player.velocity.l = -1;
                player.animation_state = player.run;
                player.animation_direction = player.left;
                break;
            case SDLK_s:
                player.velocity.d = 1;
                break;
            case SDLK_d:
                player.velocity.r = 1;
                player.animation_state = player.run;
                player.animation_direction = player.right;
                break;
            default:
                break;
        }
    }
    if (Game::event.type==SDL_KEYUP){
        switch (Game::event.key.keysym.sym){
            case SDLK_w:
                player.velocity.u = 0;
                break;
            case SDLK_a:
                player.velocity.l = 0;
                player.animation_state = player.idle;
                break;
            case SDLK_s:
                player.velocity.d = 0;
                break;
            case SDLK_d:
                player.velocity.r = 0;
                player.animation_state = player.idle;
                break;
            default:
                break;
        }
    }
}