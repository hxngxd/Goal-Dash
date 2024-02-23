#include "game.h"
#include "keyboard.h"
#include <typeinfo>

bool w = false, a = false, s = false, d = false;

void KeyboardHandler::PlayerMovement(Player & player){
    if (Game::event.type==SDL_KEYDOWN){
        switch (Game::event.key.keysym.sym){
            case SDLK_w: w = true;
                player.velocity.u = -1;
                break;
            case SDLK_a: a = true;
                player.velocity.l = -1;
                if (d) player.velocity.r = 0;
                player.animation_state = player.run;
                player.animation_direction = player.left;
                break;
            case SDLK_s: s = true;
                player.velocity.d = 1;
                break;
            case SDLK_d: d = true;
                player.velocity.r = 1;
                if (a) player.velocity.l = 0;
                player.animation_state = player.run;
                player.animation_direction = player.right;
                break;
            default:
                break;
        }
    }
    if (Game::event.type==SDL_KEYUP){
        switch (Game::event.key.keysym.sym){
            case SDLK_w: w = false;
                player.velocity.u = 0;
                break;
            case SDLK_a: a = false;
                player.velocity.l = 0;
                if (!d) player.animation_state = player.idle;
                break;
            case SDLK_s: s = false;
                player.velocity.d = 0;
                break;
            case SDLK_d: d = false;
                player.velocity.r = 0;
                if (!a) player.animation_state = player.idle;
                break;
            default:
                break;
        }
    }
}