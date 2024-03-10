#include "datalib.h"
#include "gameobject.h"
#include "game.h"

std::vector<std::vector<int>> TileMap;
std::vector<MapTile> Tiles;
std::vector<Background> Backgrounds;

MapTile::MapTile(Vector2 position, Vector2 size, Vector2 index, int type, float wait){
    this->position = position;
    this->size = size;
    this->type = type;
    this->currentFrame = 0;
    this->animation_delay = 0;
    this->animation_speed = 10;
    this->index = index;
    this->scale = 0;
    this->wait_for_animation = wait;
}

int MapTile::CreateTiles(){
    int mp_size = Screen::map_size;

    TileMap.resize(mp_size, std::vector<int>(mp_size, 0));
    std::ifstream in;
    in.open("map/1.map");
    for (int i=0;i<mp_size;i++){
        for (int j=0;j<mp_size;j++){
            in >> TileMap[i][j];
        }
    }
    in.close();

    float wait = SDL_GetTicks() + 500;
    for (int i=0;i<mp_size;i++){
        CreateATile(0, i, wait);
    }
    for (int i=1;i<mp_size;i++){
        CreateATile(i, mp_size-1, wait);
    }
    for (int i=mp_size-2;i>=0;i--){
        CreateATile(mp_size-1, i, wait);
    }
    for (int i=mp_size-2;i>0;i--){
        CreateATile(i, 0, wait);
    }

    int spawn_i, spawn_j;
    for (int i=1;i<mp_size-1;i++){
        for (int j=1;j<mp_size-1;j++){
            if (TileMap[i][j]==SPAWN){
                spawn_i = i;
                spawn_j = j;
                continue;
            }
            CreateATile(i, j, wait);
        }
    }
    
    std::cout << spawn_i << " " << spawn_j << std::endl;
    CreateATile(spawn_i, spawn_j, wait);
    return wait;
}

void MapTile::CreateATile(int i, int j, float & wait){
    if (!TileMap[i][j]) return;
    Tiles.push_back(
        MapTile(
            Vector2(j*Screen::tile_size, i*Screen::tile_size),
            Screen::tile_size,
            Vector2(i,j),
            TileMap[i][j],
            wait
        )
    );
    wait += 50;
}

void MapTile::Draw(){
    for (auto & tile : Tiles){
        if (tile.type != TileMap[tile.index.x][tile.index.y]) continue;

        float currentTicks = SDL_GetTicks();
        if (currentTicks <= tile.wait_for_animation) continue;
        
        SDL_Rect rect = Rect::reScale(tile.position, tile.size, tile.scale * 0.9);
        if (tile.scale < 0.5) tile.scale += 0.1;
        else if (tile.scale < 0.75) tile.scale += 0.05;
        else if (tile.scale < 1) tile.scale += 0.025;
        else if (tile.scale > 1) tile.scale = 1;
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
                if (currentTicks > tile.animation_delay + 1000/tile.animation_speed){
                    tile.currentFrame += 1;
                    if (tile.currentFrame >= Sprites["coin"]->maxFrames) tile.currentFrame = 0;
                    tile.animation_delay = currentTicks;
                }
                Screen::SetDrawColor(Color::yellow(255));
                SDL_RenderDrawRect(Game::renderer, &rect);
                Screen::DrawSprite(*Sprites["coin"], tile.position, tile.size, tile.scale * 0.5, tile.currentFrame, 0);
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

