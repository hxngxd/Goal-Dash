#include "datalib.h"
#include "game.h"

std::vector<Sprite> Sprite::SpriteList = {Sprite()};

const Vector2 Vector2::right(1, 0);
const Vector2 Vector2::left(-1, 0);
const Vector2 Vector2::down(0, 1);
const Vector2 Vector2::up(0, -1);

float Vector2::distance(Vector2 other){
    float dx = other.x - x;
    float dy = other.y - y;
    return sqrt(dx*dx + dy*dy);
}

void Vector2::Int(Vector2 & v){
    v.x = floor(v.x);
    v.y = floor(v.y);
}

bool Vector2::IsInRange(Vector2 & v, float minX, float maxX, float minY, float maxY){
    return minX <= v.x && v.x <= maxX && minY <= v.y && v.y <= maxY;
}

bool Rect::isCollide(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2){
    float c_x = abs(pos1.x - pos2.x) - size1.x/2 - size2.x/2;
    float c_y = abs(pos1.y - pos2.y) - size1.y/2 - size2.y/2;
    return c_x <= 0 && c_y <= 0;
}

Sprite::Sprite(const char * path, int maxFrames, Vector2 realSize){
    SDL_Surface * tmpSurface = IMG_Load(path);
    this->texture = SDL_CreateTextureFromSurface(Game::renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);
    
    this->path = path;
    this->maxFrames = maxFrames;
    this->realSize = realSize;

    if (this->texture==nullptr){
        std::cout << "Error: SDL failed to load texture " << path << " - " << SDL_GetError();
    }
    else{
        Sprite::SpriteList.push_back(*this);
        std::cout << "Sprite loaded: " << path << std::endl;
    }
}

Vector2 Rect::RectCenter(Vector2 position, Vector2 size){
    return position + size/2;
}