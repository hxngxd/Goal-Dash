#include "game.h"
#include "inputhandler.h"

void KeyboardHandler::LeftPlayerInputHandler(Player & player){
    if (SDL_GetTicks() <= player.wait_for_animation) return;
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
            case SDLK_SPACE:
                if (player.collide_down){
                    player.collide_down = false;
                    player.velocity.d = -Game::jump_speed;
                    player.position.y += player.velocity.d;
                    player.previous_state = player.current_state
                    ;
                    player.current_state = JUMP;
                    playSound("jump", jump_channel, 0);
                }
                break;
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
            case SDLK_SPACE:
                break;
            default:
                break;
        }
    }
}

void KeyboardHandler::RightPlayerInputHandler(Player & player){
    if (SDL_GetTicks() <= player.wait_for_animation) return;
    if (Game::event.type==SDL_KEYDOWN){
        switch (Game::event.key.keysym.sym){
            case SDLK_UP:
                player.key_up = true;
                break;
            case SDLK_DOWN:
                player.key_down = true;
                break;
            case SDLK_LEFT:
                player.key_left = true;
                break;
            case SDLK_RIGHT:
                player.key_right = true;
                break;
            case SDLK_KP_ENTER:
                if (player.collide_down){
                    player.collide_down = false;
                    player.velocity.d = -Game::jump_speed;
                    player.position.y += player.velocity.d;
                    player.previous_state = player.current_state;
                    player.current_state = JUMP;
                    playSound("jump", jump_channel, 0);
                }
                break;
            default:
                break;
        }
    }
    if (Game::event.type==SDL_KEYUP){
        switch (Game::event.key.keysym.sym){
            case SDLK_UP:
                player.key_up = false;
                break;
            case SDLK_DOWN:
                player.key_down = false;
                break;
            case SDLK_LEFT:
                player.key_left = false;
                break;
            case SDLK_RIGHT:
                player.key_right = false;
                break;
            case SDLK_KP_ENTER:
                break;
            default:
                break;
        }
    }
}