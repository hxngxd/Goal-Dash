#include <bits/stdc++.h>
#include "../mylib/Screen.hpp"
#include "../mylib/Render.hpp"
#include "../mylib/Math.hpp"

void Render::Init(SDL_Window * window){
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer==nullptr){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
    }
}

void Render::SetDrawColor(SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Render::Clear(SDL_Color color){
    SetDrawColor(color);
    SDL_RenderClear(renderer);
}

void Render::Display(){
    SDL_RenderPresent(renderer);
}

void Render::PointGrid(Screen & currentScreen, SDL_Color color){
    SetDrawColor(color);
    int sqr = currentScreen.width / currentScreen.scale;
    for (int i=sqr;i<currentScreen.width;i+=sqr){
        for (int j=sqr;j<currentScreen.height;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}