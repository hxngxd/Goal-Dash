#include "game.h"
#include "keyboard.h"
#include <typeinfo>

bool w = false, a = false, s = false, d = false;

void KeyboardHandler::PlayerMovement(Player & player){
    if (Game::event.type==SDL_KEYDOWN){
        switch (Game::event.key.keysym.sym){
            // case SDLK_w: w = true;
            //     player.velocity.u = -1;
            //     break;
            case SDLK_a: a = true;
                player.animation_state = player.run;
                player.animation_direction = player.left;
                player.previous_animation_state = player.animation_state;
                break;
            // case SDLK_s: s = true;
            //     player.velocity.d = 1;
            //     break;
            case SDLK_d: d = true;
                player.animation_state = player.run;
                player.animation_direction = player.right;
                player.previous_animation_state = player.animation_state;
                break;
            case SDLK_SPACE:
                player.previous_animation_state = player.animation_state;
                player.animation_state = player.jump;
                if (a) player.animation_direction = player.left;
                else if (d) player.animation_direction = player.right;

                player.velocity.d = -jump_speed;
                player.onGround = false;
                break;
            default:
                break;
        }
    }
    if (Game::event.type==SDL_KEYUP){
        switch (Game::event.key.keysym.sym){
            // case SDLK_w: w = false;
            //     player.velocity.u = 0;
            //     break;
            case SDLK_a: a = false;
                if (!d) player.animation_state = player.idle;
                else{
                    player.animation_direction = player.right;
                }
                player.previous_animation_state = player.animation_state;
                break;
            // case SDLK_s: s = false;
            //     player.velocity.d = 0;
            //     break;
            case SDLK_d: d = false;
                if (!a) player.animation_state = player.idle;
                else{
                    player.animation_direction = player.left;
                }
                player.previous_animation_state = player.animation_state;
                break;
            default:
                break;
        }
    }
}