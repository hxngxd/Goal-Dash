#include "game.h"
#include "render.h"

bool Renderer::CreateRenderer(){
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer==nullptr){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
        return false;
    }
    else{
        std::cout << "Renderer created!" << std::endl;
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
    int sqr = resolution.x/map_size;
    for (int i=sqr;i<resolution.x;i+=sqr){
        for (int j=sqr;j<resolution.y;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void Sprite::LoadSprite(const char * path, int maxFrames, Vector2 size){
    SDL_Surface * tmpSurface = IMG_Load(path);
    this->texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);
    
    this->path = path;
    this->maxFrames = maxFrames;
    this->size = size;

    if (this->texture==nullptr){
        std::cout << "Error: SDL failed to load texture " << path << " - " << SDL_GetError();
    }
    else{
        std::cout << "Sprite loaded: " << path << std::endl;
    }
}

void Renderer::DrawSprite(Sprite & sprite, Vector2 position, Vector2 size, int currentFrame, bool flip){
    SDL_Rect src = {(currentFrame%sprite.maxFrames)*sprite.size.x, 0, sprite.size.x, sprite.size.y};
    SDL_Rect dst = {position.x, position.y, size.x, size.y};
    SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}