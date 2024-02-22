#include <iostream>
#include "screen.h"

bool Screen::CreateWindow() {
    window = SDL_CreateWindow(title, x, y, width, height, fixedsize);
    if (window==nullptr){
        std::cout << "Error: Failed to open window - " << SDL_GetError();
        return false;
    }
    return true;
}

void Screen::DestroyWindow(){
    SDL_DestroyWindow(window);
}