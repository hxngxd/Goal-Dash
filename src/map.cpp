#include "game.h"

std::vector<MapTile> MapTile::Tiles;

MapTile::MapTile(Vector2 position, Vector2 size, Direction velocity, int type){
    this->position = position;
    this->size = size;
    this->velocity = velocity;
    this->type = type;
    Tiles.push_back(*this);
}

void MapTile::Create(std::vector<std::vector<int>> & map){
    int tile_size = resolution.x/map_size;
    for (int i=0;i<map_size;i++){
        for (int j=0;j<map_size;j++){
            if (!map[i][j]) continue;
            MapTile newTile(Vector2(j*tile_size, i*tile_size), Vector2(tile_size), Direction(), map[i][j]);
        }
    }
}

void MapTile::Draw(){
    for (auto tile : Tiles){
        SDL_Rect rect = {tile.position.x + 1, tile.position.y + 1, tile.size.x - 1, tile.size.y - 1};
        switch (tile.type){
            case 1:
                Renderer::SetDrawColor(Color::white(255));
                Renderer::DrawSprite(tileSprite_1, tile.position, tile.size, 0, 0);
                break;
            case 2:
                Renderer::SetDrawColor(Color::yellow(255));
                SDL_RenderDrawRect(renderer, &rect);
                break;
            default:
                break;
        }
    }
}