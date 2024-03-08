#include "datalib.h"
#include "gameobject.h"
#include "game.h"

std::vector<std::vector<int>> tilemap = {
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 4, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 4},
    {4, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 8, 8, 8, 0, 0, 4, 0, 0, 0, 4},
    {4, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 4},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {4, 8, 8, 8, 8, 8, 0, 16, 0, 2, 0, 0, 0, 0, 0, 4},
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}
};

std::vector<MapTile> Tiles;

MapTile::MapTile(Vector2 position, Vector2 size, Direction velocity, int type, Vector2 tile){
    this->position = position;
    this->size = size;
    this->velocity = velocity;
    this->type = type;
    this->currentFrame = 0;
    this->animation_delay = 0;
    this->tile = tile;
}

void MapTile::Create(){
    int map_sz = Screen::map_size;
    int tile_sz = Screen::resolution.x/map_sz;

    for (int i=0;i<map_sz;i++){
        for (int j=0;j<map_sz;j++){
            if (!tilemap[i][j]) continue;

            Tiles.push_back(MapTile(Vector2(j*tile_sz, i*tile_sz), Vector2(tile_sz), Direction(), tilemap[i][j], Vector2(i,j)));

            if (tilemap[i][j]==SPAWN){
                Game::startingPosition = Vector2(j*tile_sz, i*tile_sz);
            }
        }
    }
}

void MapTile::Draw(){
    for (auto & tile : Tiles){
        if (tile.type != tilemap[tile.tile.x][tile.tile.y]) continue;
        float currentTicks = SDL_GetTicks();
        SDL_Rect rect = {tile.position.x + 2, tile.position.y + 2, tile.size.x - 2, tile.size.y - 2};
        switch (tile.type){
            case WIN:
                Screen::SetDrawColor(Color::green(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                break;
            case SPAWN:
                Screen::SetDrawColor(Color::cyan(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                break;
            case COIN:
                if (currentTicks > tile.animation_delay + 1500/Game::animation_speed){
                    tile.currentFrame += 1;
                    if (tile.currentFrame >= Sprites["coin"]->maxFrames) tile.currentFrame = 0;
                    tile.animation_delay = currentTicks;
                }
                Screen::SetDrawColor(Color::yellow(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                Screen::DrawSprite(*Sprites["coin"], tile.position + Vector2(10), tile.size - Vector2(18), tile.currentFrame, 0);
                break;
            case DAMAGE:
                Screen::SetDrawColor(Color::red(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                break;
            default:
                break;
        }
    }
}

void MapTile::Update(){
    Draw();
}