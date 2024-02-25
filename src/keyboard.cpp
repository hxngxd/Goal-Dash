#include "game.h"
#include "keyboard.h"
#include <typeinfo>

bool KeyboardHandler::key_w = false;
bool KeyboardHandler::key_a = false;
bool KeyboardHandler::key_s = false;
bool KeyboardHandler::key_d = false;
bool KeyboardHandler::key_space = false;

void KeyboardHandler::InputHandler(){
    if (Game::event.type==SDL_KEYDOWN){
        switch (Game::event.key.keysym.sym){
            case SDLK_w:
                key_w = true;
                break;
            case SDLK_s:
                key_s = true;
                break;
            case SDLK_a:
                key_a = true;
                break;
            case SDLK_d:
                key_d = true;
                break;
            case SDLK_SPACE:
                key_space = true;
                break;
            default:
                break;
        }
    }
    if (Game::event.type==SDL_KEYUP){
        switch (Game::event.key.keysym.sym){
            case SDLK_w:
                key_w = false;
                break;
            case SDLK_s:
                key_s = false;
                break;
            case SDLK_a:
                key_a = false;
                break;
            case SDLK_d:
                key_d = false;
                break;
            case SDLK_SPACE:
                key_space = false;
                break;
            default:
                break;
        }
    }
}