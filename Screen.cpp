#include <bits/stdc++.h>
#include "mylib/Screen.hpp"

Screen::Screen() {
    window = SDL_CreateWindow(title, x, y, width, height, fixedsize);

    if (window==NULL){
        std::cout << "Error: Failed to open window - " << SDL_GetError();
    }
}

void Screen::CleanUp(){
    SDL_DestroyWindow(window);
    std::cout << "CLOSED";
}