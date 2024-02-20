#include <bits/stdc++.h>
#include "mylib/Render.hpp"
#include "mylib/Screen.hpp"
#include "mylib/Entity.hpp"

Render::Render(SDL_Window * window){
    renderer = SDL_CreateRenderer(window, -1, renderer_flags);

    if (renderer==NULL){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
    }
}

void Render::Clear(){
    SDL_RenderClear(renderer);
}

void Render::RenderIt(Entity & entity){
    SDL_Rect dst = {64, 64, 256, 256};
    SDL_RenderCopy(renderer, entity.texture, &entity.rect, &dst);
}

void Render::Display(){
    SDL_RenderPresent(renderer);
}

SDL_Texture * Render::loadTexture(const char * path){
    SDL_Texture * texture = NULL;
    texture = IMG_LoadTexture(renderer, path);

    if (texture==NULL){
        std::cout << "Error: Failed to load texture - " << SDL_GetError();
    }

    return texture;
}

void Render::CleanUp(){
    SDL_DestroyRenderer(renderer);
}