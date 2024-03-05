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
            if (map[i][j]==-1) Game::startingPosition = Vector2(j*tile_size, i*tile_size);
        }
    }
}

void MapTile::Draw(){
    for (auto & tile : MapTile::Tiles){
        if (tile.type != Game::map[tile.tile.x][tile.tile.y]) continue;
        float currentTicks = SDL_GetTicks();
        SDL_Rect rect = {tile.position.x + 2, tile.position.y + 2, tile.size.x - 2, tile.size.y - 2};
        switch (tile.type){
            case win:
                if (Game::view_mode){
                    Renderer::SetDrawColor(Color::green(255));
                    SDL_RenderDrawRect(Game::renderer, &rect);
                }
                break;
            case spawn:
                if (Game::view_mode){
                    Renderer::SetDrawColor(Color::cyan(255));
                    SDL_RenderDrawRect(Game::renderer, &rect);
                }
                break;
            case wall:
                if (Game::view_mode){
                    Renderer::SetDrawColor(Color::white(255));
                    SDL_RenderDrawRect(Game::renderer, &rect);
                }
                else{
                    Renderer::DrawSprite(Sprites.wall, tile.position, tile.size, 0, 0);
                }
                break;
            case coin:
                if (currentTicks > tile.animation_delay + 1500/Game::animation_speed){
                    tile.currentFrame += 1;
                    if (tile.currentFrame >= Sprites.coin.maxFrames) tile.currentFrame = 0;
                    tile.animation_delay = currentTicks;
                }
                if (Game::view_mode){
                    Renderer::SetDrawColor(Color::yellow(255));
                    SDL_RenderDrawRect(Game::renderer, &rect);
                }
                Renderer::DrawSprite(Sprites.coin, tile.position + Vector2(10), tile.size - Vector2(18), tile.currentFrame, 0);
                break;
            case damage:
                if (Game::view_mode){
                    Renderer::SetDrawColor(Color::red(255));
                    SDL_RenderDrawRect(Game::renderer, &rect);
                }
                break;
            default:
                break;
        }
    }
}

void MapTile::Update(){
    Draw();
}