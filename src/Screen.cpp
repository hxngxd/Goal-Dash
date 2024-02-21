#include <bits/stdc++.h>
#include "../mylib/Screen.hpp"

void Screen::Init() {
    window = SDL_CreateWindow(title, x, y, width, height, fixedsize);
    if (window==nullptr){
        std::cout << "Error: Failed to open window - " << SDL_GetError();
    }
}