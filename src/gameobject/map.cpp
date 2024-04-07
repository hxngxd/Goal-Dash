#include "../datalib/sprite.h"
#include "../event/input.h"
#include "../event/ui.h"
#include "../game.h"
#include "gameobject.h"
#include <fstream>
#include <set>

std::vector<std::vector<std::pair<int, MapTile *>>> TileMap;
std::vector<Background> Backgrounds;
int MapTile::nEmptyTiles = 0;

Vector2 MapTile::SpawnTile;
Vector2 MapTile::WinTile;

bool MapTile::isMakingMap = false;
int CurrentDrawingType = EMPTY;
bool isRight = false;
int x0 = 9;
int y0 = 4;

MapTile::MapTile(Vector2 position, Vector2 size)
{
    this->position = position;
    this->size = size;
    this->currentFrame = 0;
    this->animation_delay = 0;
    this->animation_speed = 10;
    this->scale = 0;
}

void MapTile::CreateBorder()
{
    int ms = Screen::map_size;
    TileMap.clear();
    TileMap.resize(ms, std::vector<std::pair<int, MapTile *>>(ms, std::make_pair(0, nullptr)));

    float wait = 500;

    for (int i = 0; i < ms; i++)
    {
        TileMap[0][i].first = WALL;
        CreateATile(0, i, wait);
    }

    for (int i = 1; i < ms; i++)
    {
        TileMap[i][ms - 1].first = WALL;
        CreateATile(i, ms - 1, wait);
    }

    for (int i = ms - 2; i >= 0; i--)
    {
        TileMap[ms - 1][i].first = WALL;
        CreateATile(ms - 1, i, wait);
    }

    for (int i = ms - 2; i > 0; i--)
    {
        TileMap[i][0].first = WALL;
        CreateATile(i, 0, wait);
    }
}

void MapTile::CreateTiles(int map)
{
    int mp_size = Screen::map_size;

    std::ifstream in;
    in.open("map/" + std::to_string(map) + ".map");
    nEmptyTiles = 0;

    // temporary code
    if (!in.good())
    {
        game->Stop();
        return;
    }

    for (int i = 1; i < mp_size - 1; i++)
    {
        for (int j = 1; j < mp_size - 1; j++)
        {
            in >> TileMap[i][j].first;
            if (TileMap[i][j].first)
                nEmptyTiles++;
        }
    }
    in.close();

    float wait = 0.1;
    for (int i = 1; i < mp_size - 1; i++)
    {
        for (int j = 1; j < mp_size - 1; j++)
        {
            if (TileMap[i][j].first == SPAWN)
            {
                SpawnTile = Vector2(i, j);
                continue;
            }
            else if (TileMap[i][j].first == WIN)
            {
                WinTile = Vector2(i, j);
                continue;
            }
            CreateATile(i, j, wait);
        }
    }

    CreateATile(SpawnTile.x, SpawnTile.y, wait);
    TileMap[WinTile.x][WinTile.y].first = 0;
}

void MapTile::CreateATile(int i, int j, float &wait)
{
    if (!TileMap[i][j].first)
        return;

    TileMap[i][j].second = new MapTile(Vector2(j * Screen::tile_size, i * Screen::tile_size), Screen::tile_size);

    if (TileMap[i][j].first == COIN)
        Game::Properties["coin"].i++;

    LinkedFunction *lf =
        new LinkedFunction(std::bind(TransformValue<float>, &TileMap[i][j].second->scale,
                                     Game::Properties["tile_scale"].f * (TileMap[i][j].first == COIN ? 0.6f : 1.0f),
                                     Game::Properties["rescale_speed"].f),
                           wait);
    lf->Execute();
    wait += Game::Properties["map_animation_delay"].f;
}

void MapTile::DeleteTiles()
{
    float wait = 0.1;
    auto post_func = [](int i, int j) {
        TileMap[i][j].first = 0;
        if (TileMap[i][j].second)
        {
            delete TileMap[i][j].second;
            TileMap[i][j].second = nullptr;
        }
    };

    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            if (!TileMap[i][j].second)
                continue;
            LinkedFunction *lf = new LinkedFunction(std::bind(TransformValue<float>, &TileMap[i][j].second->scale, 0.0f,
                                                              Game::Properties["rescale_speed"].f),
                                                    wait);
            lf->Execute();
            wait += Game::Properties["map_animation_delay"].f;
        }
    }
}

void MapTile::Update()
{
    for (int i = 0; i < Screen::map_size; i++)
    {
        for (int j = 0; j < Screen::map_size; j++)
        {
            if (!TileMap[i][j].first || !TileMap[i][j].second)
                continue;

            if (!TileMap[i][j].second->scale)
                continue;

            if (Game::Properties["map"].i && (i == 0 || i == Screen::map_size - 1 && j <= 2))
                continue;

            SDL_Rect rect =
                Rect::Rescale(TileMap[i][j].second->position, TileMap[i][j].second->size, TileMap[i][j].second->scale);

            switch (TileMap[i][j].first)
            {
            case WIN:
                Animate(TileMap[i][j].second, "win");
                break;
            case SPAWN:
                Animate(TileMap[i][j].second, "spawn");
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(Game::Properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                // Animate(TileMap[i][j].second, "wall");
                break;
            case COIN:
                Screen::SetDrawColor(Color::yellow(Game::Properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                Animate(TileMap[i][j].second, "coin");
                break;
            case DAMAGE:
                Screen::SetDrawColor(Color::red(Game::Properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                // Animate(TileMap[i][j].second, "damage");
                break;
            default:
                break;
            }
        }
    }

    if (isMakingMap)
    {
        Vector2 mouseTile = Int(Vector2(mousePosition.x / Screen::tile_size, mousePosition.y / Screen::tile_size));
        if (InRange(mouseTile, Vector2(1), Vector2(14)))
        {
            mouseTile *= Screen::tile_size;
            SDL_Rect mouseRect = {mouseTile.x, mouseTile.y, Screen::tile_size, Screen::tile_size};
            Screen::SetDrawColor(Color::white(64));
            SDL_RenderFillRect(Game::renderer, &mouseRect);
        }
        // bool up_touched = false;
        // Vector2 touched_pos(-1, -1);
        // Vector2 touched_tile(-1, -1);
        // std::set<Vector2> tiles;

        // Screen::SetDrawColor(Color::white(255));

        // float v0x = Game::Properties["player_move_speed"].f;
        // float v0y = Game::Properties["player_jump_speed"].f;
        // float g = Game::Properties["gravity"].f;

        // float A = g / (2.0f * v0x * v0x);
        // float B = v0y / v0x;

        // float x00 = (x0 + 0.5f) * Screen::tile_size;
        // float y00 = (y0 + 0.5f) * Screen::tile_size;
        // float x, y;

        // for (x = x00; isRight ? x < Game::Properties["resolution"].i : x > 0; isRight ? x++ : x--)
        // {
        //     float C = x00 - (isRight ? 0 : (2.0f * v0x * v0y) / g);
        //     y = A * (x - C) * (x - C) - B * (x - C) + y00;

        //     float dx = 2.0f * A * (x - C) - B;
        //     bool up = (isRight && dx < 0) || (!isRight && dx > 0);

        //     Vector2 current_tile = Int(Vector2(x, y) / Screen::tile_size);
        //     if (up && TileMap[current_tile.y][current_tile.x].first == WALL && !up_touched)
        //     {
        //         up_touched = true;
        //         touched_pos.x = x;
        //         touched_tile = current_tile;
        //     }

        //     if (up_touched)
        //     {
        //         float C1 = C + ((v0x * v0y) / g - abs(touched_pos.x - x00)) * (isRight ? -1 : 1);
        //         touched_pos.y = A * (touched_pos.x - C) * (touched_pos.x - C) - B * (touched_pos.x - C) + y00;
        //         y = A * (x - C1) * (x - C1) - B * (x - C1) + y00 +
        //             abs((v0y * v0y) / (2.0f * g) - abs(y00 - touched_pos.y)) + 3;

        //         if (!(touched_tile.x * Screen::tile_size <= x <= (touched_tile.x + 1) * Screen::tile_size &&
        //               y >= (touched_tile.y + 1) * Screen::tile_size))
        //             break;

        //         dx = 2.0f * A * (x - C1) - B;
        //         up = (isRight && dx < 0) || (!isRight && dx > 0);

        //         Screen::SetDrawColor(Color::green(255));
        //     }

        //     SDL_RenderDrawPoint(Game::renderer, x, y);

        //     current_tile = Int(Vector2(x, y) / Screen::tile_size);
        //     if (TileMap[current_tile.y][current_tile.x].first == 4 && !up &&
        //         abs(x - touched_pos.x) >= Screen::tile_size)
        //         break;
        //     if (TileMap[current_tile.y][current_tile.x].first == COIN)
        //     {
        //         if (tiles.find(current_tile) == tiles.end())
        //             tiles.insert(current_tile);
        //     }
        // }

        // for (auto &tile : tiles)
        // {
        //     print(tile);
        // }
    }
}

void MapHUD()
{
    Canvas *hcv1 = new Canvas("horizontalhub1", Vector2(), Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    Button *clear = new Button(
        "clear", "Clear", Vector2(Screen::tile_size * 2, 0), []() { MapTile::DeleteTiles(); }, 25);
    Button *save = new Button(
        "save", "Save", Vector2(Screen::tile_size * 2, 0), []() {}, 25);
    Button *goright = new Button(
        "goright", "Go ->", Vector2(Screen::tile_size, 0), []() { x0++; }, 25);
    Button *goleft = new Button(
        "goleft", "Go <-", Vector2(Screen::tile_size, 0), []() { x0--; }, 25);
    Button *goup = new Button(
        "goup", "Go ^", Vector2(Screen::tile_size, 0), []() { y0--; }, 25);
    Button *godown = new Button(
        "godown", "Go v", Vector2(Screen::tile_size, 0), []() { y0++; }, 25);
    hcv1->AddComponents({"clear", "save", "goleft", "goright", "goup", "godown"});

    Canvas *hcv2 = new Canvas("horizontalhub2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                              Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    Button *drawEmpty = new Button(
        "empty", "Empty", Vector2(Screen::tile_size * 2, 0), []() { CurrentDrawingType = EMPTY; }, 25);
    Button *drawWall = new Button(
        "wall", "Wall", Vector2(Screen::tile_size * 2, 0), []() { CurrentDrawingType = WALL; }, 25);
    Button *drawCoin = new Button(
        "coin", "Coin", Vector2(Screen::tile_size * 2, 0), []() { CurrentDrawingType = COIN; }, 25);
    Button *drawSpawn = new Button(
        "spawn", "Spawn", Vector2(Screen::tile_size * 2, 0), []() { CurrentDrawingType = SPAWN; }, 25);
    Button *drawWin = new Button(
        "win", "Win", Vector2(Screen::tile_size * 2, 0), []() { CurrentDrawingType = WIN; }, 25);
    hcv2->AddComponents({"empty", "wall", "coin", "spawn", "win"});
}

Background::Background(std::string name, float scale)
{
    this->opacity = 64;
    this->toggle = true;
    this->name = name;
    this->position = Vector2(0);
    this->size = Screen::resolution;
    this->currentFrame = 0;
    this->maxFrames = Sprites[name]->maxFrames;
    this->scale = scale;
}

bool Background::loadBackground(std::string name, std::string path, int maxFrames, Vector2 realSize, float scale)
{
    if (!LoadSprite(name, path, maxFrames, realSize))
        return 0;
    print("creating background", name);
    Backgrounds.push_back(Background(name, scale));
    print("background", name, "created");
    SDL_SetTextureBlendMode(Sprites[name]->texture, SDL_BLENDMODE_BLEND);
    return 1;
}

void Background::Update()
{
    Background &bg0 = Backgrounds[0];
    Background &bg1 = Backgrounds[1];
    Background &bg2 = Backgrounds[2];

    bg0.opacity = 150;
    SetSpriteOpacity(bg0.name, bg0.opacity);
    DrawSprite(bg0.name, bg0.position, bg0.size, bg0.scale, bg0.maxFrames, 0);

    if (bg2.toggle)
    {
        bg2.opacity += 2;
        if (bg2.opacity >= 248)
            bg2.toggle = false;
    }
    else
    {
        bg2.opacity -= 2;
        if (bg2.opacity <= 8)
            bg2.toggle = true;
    }

    SetSpriteOpacity(bg2.name, bg2.opacity);
    DrawSprite(bg2.name, bg2.position, bg2.size, bg2.scale, bg2.maxFrames, 0);

    bg1.opacity = 256 - bg2.opacity;
    SetSpriteOpacity(bg1.name, bg1.opacity);
    DrawSprite(bg1.name, bg1.position, bg1.size, bg1.scale, bg1.maxFrames, 1);
}

void Background::Move(Vector2 velocity, int index, float ratio)
{
    Background &bg = Backgrounds[index];

    if (velocity.x)
    {
        float bound = (1 - bg.scale) * bg.size.x * 0.5;
        bg.position.x += velocity.x * ratio;
        bg.position.x = Clamp(bg.position.x, bound, -bound);
    }

    if (velocity.y)
    {
        float bound = (1 - bg.scale) * bg.size.y * 0.5;
        bg.position.y += velocity.y * ratio;
        bg.position.y = Clamp(bg.position.y, bound, -bound);
    }
}