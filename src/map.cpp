#include "game.h"
#include "gameobject.h"

std::vector<MapTile> MapTile::Tiles;

MapTile::MapTile(Vector2 position, Vector2 size, Direction velocity, int type, Vector2 tile){
    this->position = position;
    this->size = size;
    this->velocity = velocity;
    this->type = type;
    this->currentFrame = 0;
    this->animation_delay = 0;
    this->tile = tile;
}

void MapTile::Create(std::vector<std::vector<int>> & map){
    int tile_size = Screen::resolution.x/Screen::map_size;
    for (int i=0;i<Screen::map_size;i++){
        for (int j=0;j<Screen::map_size;j++){
            if (!map[i][j]) continue;
            Tiles.push_back(MapTile(Vector2(j*tile_size, i*tile_size), Vector2(tile_size), Direction(), map[i][j], Vector2(i,j)));
        }
    }
}

void MapTile::Draw(){
    for (auto & tile : MapTile::Tiles){
        if (tile.type != Game::map[tile.tile.x][tile.tile.y]) continue;
        float currentTicks = SDL_GetTicks();
        SDL_Rect rect = {tile.position.x, tile.position.y, tile.size.x, tile.size.y};
        switch (tile.type){
            case wall:
                Renderer::SetDrawColor(Color::white(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                // Renderer::DrawSprite(Sprite::SpriteList[wall], tile.position, tile.size, 0, 0);
                break;
            case coin:
                if (currentTicks > tile.animation_delay + 1500/Game::animation_speed){
                    tile.currentFrame += 1;
                    if (tile.currentFrame >= Sprite::SpriteList[coin].maxFrames) tile.currentFrame = 0;
                    tile.animation_delay = currentTicks;
                }
                Renderer::SetDrawColor(Color::yellow(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                Renderer::DrawSprite(Sprite::SpriteList[coin], tile.position + Vector2(10), tile.size - Vector2(20), tile.currentFrame, 0);
                break;
            default:
                break;
        }
    }
}

void MapTile::Update(){
    Draw();
}