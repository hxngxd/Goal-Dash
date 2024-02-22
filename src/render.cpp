#include "game.h"
#include "render.h"

bool Renderer::CreateRenderer(){
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer==nullptr){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
        return false;
    }
    return true;
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

void Renderer::PointGrid(SDL_Color color){
    SetDrawColor(color);
    int sqr = width/map_size;
    for (int i=sqr;i<width;i+=sqr){
        for (int j=sqr;j<height;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

SDL_Texture * TextureManager::LoadTexture(const char * name, const char * path){
    SDL_Surface * tmpSurface = IMG_Load(path);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    if (texture==nullptr){
        std::cout << "Error: SDL failed to load texture " << name << " - " << SDL_GetError();
    }
    std::cout << "Loaded: " << name << std::endl;
    return texture;
}