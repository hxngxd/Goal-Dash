#include "datalib.h"
#include "gameobject.h"
#include "game.h"

std::vector<std::vector<int>> tilemap = {
    {4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},
    {4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4},
    {4,  0,  0,  0,  4,  8,  0, 16,  0,  0,  0,  0,  0,  0,  0,  4},
    {4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 16,  8,  0,  4},
    {4,  0,  8,  0,  0,  0,  4,  0,  0,  0, 16,  0,  0,  0,  0,  4},
    {4, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  4,  4,  4},
    {4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4},
    {4,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  4},
    {4,  0,  4,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  4},
    {4,  0,  0,  0,  0,  0,  8,  0,  8,  0,  0,  4,  0,  0,  0,  4},
    {4,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4},
    {4,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  8,  0,  4},
    {4,  0,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  4,  4,  0,  4},
    {4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4},
    {4,  0,  8,  0,  0,  0,  0, 16,  0,  2,  0,  0,  0,  0,  0,  4},
    {4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4}
};

std::vector<MapTile> Tiles;

MapTile::MapTile(Vector2 position, Vector2 size, Direction velocity, int type, Vector2 tile, float wait_for_animation){
    this->position = position;
    this->size = size;
    this->velocity = velocity;
    this->type = type;
    this->currentFrame = 0;
    this->animation_delay = 0;
    this->tile = tile;
    this->size_animation = (Screen::resolution.x / Screen::map_size) / 2;
    this->wait_for_animation = wait_for_animation;
}

int MapTile::Create(){
    int mpsz = Screen::map_size;
    int tile_sz = Screen::resolution.x/mpsz;
    int wait = 100;
    for (int i=0;i<mpsz;i++) CreateATile(0, i, wait);
    for (int i=1;i<mpsz;i++) CreateATile(i, mpsz-1, wait);
    for (int i=mpsz-2;i>=0;i--) CreateATile(mpsz-1, i, wait);
    for (int i=mpsz-2;i>0;i--) CreateATile(i, 0, wait);

    int spawn_i, spawn_j;
    for (int i=1;i<mpsz-1;i++){
        for (int j=1;j<mpsz-1;j++){
            if (tilemap[i][j]==SPAWN){
                spawn_i = i;
                spawn_j = j;
                continue;
            }
            CreateATile(i, j, wait);
        }
    }
    
    Game::startingPosition = Vector2(spawn_j*tile_sz, spawn_i*tile_sz);
    CreateATile(spawn_i, spawn_j, wait);
    return wait;
}

void MapTile::CreateATile(int i, int j, int & wait){
    if (!tilemap[i][j]) return;
    int tile_sz = Screen::resolution.x/Screen::map_size;
    Tiles.push_back(MapTile(Vector2(j*tile_sz, i*tile_sz), Vector2(tile_sz), Direction(), tilemap[i][j], Vector2(i,j), wait));
    wait += 100;
}

void MapTile::Draw(){
    for (auto & tile : Tiles){
        if (tile.type != tilemap[tile.tile.x][tile.tile.y]) continue;
        float currentTicks = SDL_GetTicks();
        SDL_Rect rect = {tile.position.x + tile.size_animation.x + 1, tile.position.y + tile.size_animation.y + 1, tile.size.x - 2*tile.size_animation.x - 2, tile.size.y - 2*tile.size_animation.y - 2};
        if (tile.size_animation.x > 0 && currentTicks > tile.wait_for_animation) tile.size_animation -= Vector2(0.75);
        if (tile.size_animation.x < 0) tile.size_animation = 0;
        if (currentTicks <= tile.wait_for_animation) continue;
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
                Screen::DrawSprite(*Sprites["coin"], tile.position + Vector2(10) + tile.size_animation, tile.size - Vector2(20) - tile.size_animation * 2, tile.currentFrame, 0);
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