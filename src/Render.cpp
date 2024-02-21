#include <bits/stdc++.h>
#include "../mylib/Render.hpp"

void Render::Init(SDL_Window * window){
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer==nullptr){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
    }
}

void Render::Clear(){
    SDL_RenderClear(renderer);
}

void Render::Display(){
    SDL_RenderPresent(renderer);
}

void Render::SetBackgroundColor(SDL_Color & color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}