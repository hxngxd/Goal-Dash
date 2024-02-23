#include "game.h"
#include "screen.h"

bool Screen::CreateWindow() {
    window = SDL_CreateWindow(title, x, y, resolution.x, resolution.y, fixedsize);
    if (window==nullptr){
        std::cout << "Error: Failed to open window - " << SDL_GetError();
        return false;
    }
    else{
        std::cout << "Window created!" << std::endl;
    }
    return true;
}