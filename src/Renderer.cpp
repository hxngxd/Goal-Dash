#include <bits/stdc++.h>
#include "../mylib/Screen.hpp"
#include "../mylib/Renderer.hpp"
#include "../mylib/Math.hpp"

void Renderer::Init(SDL_Window * window){
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer==nullptr){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
    }
}

SDL_Renderer * Renderer::getRenderer(){
    return this->renderer;
}

void Renderer::SetDrawColor(SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Renderer::Clear(SDL_Color color){
    SetDrawColor(color);
    SDL_RenderClear(renderer);
}

void Renderer::Display(){
    SDL_RenderPresent(renderer);
}

void Renderer::RenderGameObject(GameObject & gameObject){
    SDL_RenderCopy(renderer, gameObject.getTexture(), & gameObject.src, & gameObject.dst);
}

void Renderer::PointGrid(SDL_Color color){
    SetDrawColor(color);
    int sqr = Screen::width / Screen::scale;
    for (int i=sqr;i<Screen::width;i+=sqr){
        for (int j=sqr;j<Screen::height;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

SDL_Texture * TextureManager::LoadTexture(const char * name, const char * path, SDL_Renderer * renderer){
    SDL_Surface * tmpSurface = IMG_Load(path);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);
    if (texture==nullptr){
        std::cout << "Error: SDL failed to load texture " << name << " - " << SDL_GetError();
    }
    return texture;
}