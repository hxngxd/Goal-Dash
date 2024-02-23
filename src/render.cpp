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

void Sprite::LoadSprite(const char * path, int frames, Vector2 res){
    SDL_Surface * tmpSurface = IMG_Load(path);
    this->texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);
    
    this->path = path;
    this->frames = frames;
    this->res = res;

    if (this->texture==nullptr){
        std::cout << "Error: SDL failed to load texture " << path << " - " << SDL_GetError();
    }
    else{
        std::cout << "Sprite loaded: " << path << std::endl;
    }
}

void Renderer::DrawSprite(Sprite & sprite, Vector2 position, Vector2 displayRes, int frame, bool flipped){
    SDL_Rect src = {(frame%sprite.frames)*sprite.res.x, 0, sprite.res.x, sprite.res.y};
    SDL_Rect dst = {position.x, position.y, displayRes.x, displayRes.y};
    SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, NULL, (flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}