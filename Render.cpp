#include <bits/stdc++.h>
#include "mylib/Render.hpp"

Render::Render(SDL_Window * window){
    renderer = SDL_CreateRenderer(window, -1, renderer_flags);

    if (renderer==NULL){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
    }
}

void Render::Clear(){
    SDL_RenderClear(renderer);
}

void Render::Display(){
    SDL_RenderPresent(renderer);
}