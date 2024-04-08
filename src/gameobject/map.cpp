#include "../datalib/PerlinNoise/PerlinNoise.hpp"
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

Vector2 MapMaking::mouseTile(-1);
int MapMaking::currentDrawingType = EMPTY;
bool MapMaking::drawSpawn = false, MapMaking::drawWin = false;

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

void MapTile::CreateATile(int i, int j, float &wait, bool animation)
{
    if (!TileMap[i][j].first)
        return;

    TileMap[i][j].second = new MapTile(Vector2(j * Screen::tile_size, i * Screen::tile_size), Screen::tile_size);

    if (TileMap[i][j].first == COIN)
        Game::Properties["coin"].i++;

    if (animation)
    {
        LinkedFunction *lf =
            new LinkedFunction(std::bind(TransformValue<float>, &TileMap[i][j].second->scale,
                                         Game::Properties["tile_scale"].f * (TileMap[i][j].first == COIN ? 0.6f : 1.0f),
                                         Game::Properties["rescale_speed"].f),
                               wait);
        lf->Execute();
    }
    else
    {
        TileMap[i][j].second->scale = (TileMap[i][j].first == COIN ? 0.6f : 1.0f);
    }
    wait += Game::Properties["map_animation_delay"].f;
}

void MapTile::DeleteTiles()
{
    float wait = 0.1;

    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            DeleteATile(i, j, wait);
        }
    }
}

void MapTile::DeleteATile(int i, int j, float &wait, bool animation)
{
    if (!TileMap[i][j].first || !TileMap[i][j].second)
        return;
    if (animation)
    {
        LinkedFunction *lf = new LinkedFunction(
            std::bind(TransformValue<float>, &TileMap[i][j].second->scale, 0.0f, Game::Properties["rescale_speed"].f),
            wait);
        lf->NextFunction(std::bind(
            [](int i, int j) {
                TileMap[i][j].first = 0;
                if (TileMap[i][j].second)
                {
                    delete TileMap[i][j].second;
                    TileMap[i][j].second = nullptr;
                }
                return 1;
            },
            i, j));
        lf->Execute();
    }
    else
    {
        TileMap[i][j].first = 0;
        if (TileMap[i][j].second)
        {
            delete TileMap[i][j].second;
            TileMap[i][j].second = nullptr;
        }
    }
    wait += Game::Properties["map_animation_delay"].f;
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
        Vector2 currentMouseTile = Int(mousePosition / Screen::tile_size);
        if (InRange(currentMouseTile, Vector2(1), Vector2(Screen::map_size - 2)))
        {
            MapMaking::mouseTile = currentMouseTile;
            currentMouseTile *= Screen::tile_size;
            SDL_Rect mouseRect = {currentMouseTile.x, currentMouseTile.y, Screen::tile_size, Screen::tile_size};
            switch (MapMaking::currentDrawingType)
            {
            case WIN:
                Screen::SetDrawColor(Color::green(64));
                break;
            case SPAWN:
                Screen::SetDrawColor(Color::blue(64));
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(64));
                break;
            case COIN:
                Screen::SetDrawColor(Color::yellow(64));
                break;
            case DAMAGE:
                Screen::SetDrawColor(Color::red(64));
                break;
            case EMPTY:
                Screen::SetDrawColor(Color::black(128));
                break;
            default:
                break;
            }
            SDL_RenderFillRect(Game::renderer, &mouseRect);
        }
        else
        {
            MapMaking::mouseTile = Vector2(-1);
        }
    }

    bool isRight = false;
    bool touched = false;
    Vector2 touched_pos(-1), touched_tile(-1);

    Vector2 v0(Game::Properties["player_move_speed"].f, Game::Properties["player_jump_speed"].f);
    float g = Game::Properties["gravity"].f;

    Vector2 p0 = mousePosition;
    Vector2 p, pt;

    float A = g / (2.0f * v0.x * v0.x);
    float B = v0.y / v0.x;
    float R = (2.0f * v0.x * v0.y) / g;
    float C = p0.x - (isRight ? 0 : R);
    float D;
    float h = (v0.y * v0.y) / (2.0f * g);

    float low = Screen::tile_size;
    float high = Game::Properties["resolution"].i - Screen::tile_size;

    Screen::SetDrawColor(Color::green(255));
    for (p.x = p0.x; isRight ? p.x <= high : p.x >= low; isRight ? p.x++ : p.x--)
    {
        if (!touched)
        {
            p.y = A * (p.x - C) * (p.x - C) - B * (p.x - C) + p0.y;
            D = 2.0f * A * (p.x - C) - B;
        }
        else
        {
            float dx = R / 2.0f - abs(pt.x - p0.x);
            float dy = h - abs(p0.y - pt.y);
            float C1 = C + (dx * (isRight ? -1 : 1));
            p.y = A * (p.x - C1) * (p.x - C1) - B * (p.x - C1) + p0.y + dy;
            D = 2.0f * A * (p.x - C1) - B + 0.5f;
        }

        bool up = (isRight == (D < 0));

        Vector2 current = Int(Vector2(p.x, p.y) / Screen::tile_size);
        if (!InRange(current.y, 0, Screen::map_size - 1))
            break;
        int &current_type = TileMap[current.y][current.x].first;
        if (current_type == WALL)
        {
            if (up && !touched)
            {
                touched = true;
                pt = p;
                Vector2 tmp_p = current * Screen::tile_size;
                bool a = tmp_p.y + Screen::tile_size * 0.5f <= pt.y && pt.y <= tmp_p.y + Screen::tile_size;
                float tmp_dy = tmp_p.y + Screen::tile_size - pt.y;
                bool b = tmp_p.x + tmp_dy <= pt.x && pt.x <= tmp_p.x + Screen::tile_size - tmp_dy;
                if (!a || !b)
                    break;
            }
            if (!up)
            {
                break;
            }
        }
        SDL_RenderDrawPoint(Game::renderer, p.x, p.y);
    }

    if (mouseLeft)
    {
        int mx = MapMaking::mouseTile.x;
        int my = MapMaking::mouseTile.y;
        float wait = 0;
        int &type = MapMaking::currentDrawingType;
        if (type)
        {
            if (MapMaking::mouseTile != Vector2(-1) && TileMap[my][mx].first == EMPTY)
            {
                if (type == SPAWN && !MapMaking::drawSpawn)
                {
                    TileMap[my][mx].first = type;
                    CreateATile(my, mx, wait, 0);
                    MapMaking::drawSpawn = true;
                }
                else if (type == WIN && !MapMaking::drawWin)
                {
                    TileMap[my][mx].first = type;
                    CreateATile(my, mx, wait, 0);
                    MapMaking::drawWin = true;
                }
                else if (type != SPAWN && type != WIN)
                {
                    TileMap[my][mx].first = type;
                    CreateATile(my, mx, wait, 0);
                }
            }
        }
        else
        {
            if (MapMaking::mouseTile != Vector2(-1) && TileMap[my][mx].first != EMPTY)
            {
                if (TileMap[my][mx].first == SPAWN)
                    MapMaking::drawSpawn = false;
                else if (TileMap[my][mx].first == WIN)
                    MapMaking::drawWin = false;
                DeleteATile(my, mx, wait, 0);
            }
        }
    }
}

void MapMaking::Random()
{
    ((Button *)UIs["random"])->enabled = false;
    MapTile::DeleteTiles();
    LinkedFunction *lf = new LinkedFunction(
        []() {
            MapMaking::drawSpawn = MapMaking::drawWin = false;
            std::vector<std::vector<bool>> visitable(Screen::map_size, std::vector<bool>(Screen::map_size, false));

            // Generate walls
            bool valid = false;
            while (!valid)
            {
                MapTile::nEmptyTiles = 0;
                const siv::PerlinNoise::seed_type seed = IntegralRandom<Uint32>(0, UINT32_MAX);
                const siv::PerlinNoise perlin{seed};
                int ei, ej;
                for (int i = 1; i < Screen::map_size - 1; i++)
                {
                    for (int j = 1; j < Screen::map_size - 1; j++)
                    {
                        float v = perlin.octave2D_01(j, i, 8) * 100.0f;
                        if (v > 64)
                        {
                            TileMap[i][j].first = WALL;
                            MapTile::nEmptyTiles++;
                        }
                        else
                        {
                            TileMap[i][j].first = EMPTY;
                            ei = i;
                            ej = j;
                        }
                    }
                }
                valid = Validation(ei, ej);
            }

            // Generate spawn
            int spawn_i = IntegralRandom<int>(1, 14);
            int spawn_j = IntegralRandom<int>(1, 14);
            while (TileMap[spawn_i][spawn_j].first != EMPTY)
            {
                spawn_i = IntegralRandom<int>(1, 14);
                spawn_j = IntegralRandom<int>(1, 14);
            }
            TileMap[spawn_i][spawn_j].first = SPAWN;
            while (TileMap[spawn_i][spawn_j].first != WALL && spawn_i < Screen::map_size - 1)
            {
                visitable[spawn_i][spawn_j] = true;
                spawn_i++;
            }

            // Generate coins

            for (int i = 0; i < 16; i++)
            {
                for (int j = 0; j < 16; j++)
                {
                    std::cout << visitable[i][j] << " ";
                }
                print("");
            }

            float wait = 0.1f;
            for (int i = 1; i < Screen::map_size - 1; i++)
            {
                for (int j = 1; j < Screen::map_size - 1; j++)
                {
                    MapTile::CreateATile(i, j, wait);
                }
            }

            LinkedFunction *lf = new LinkedFunction(
                []() {
                    ((Button *)UIs["random"])->enabled = true;
                    return 1;
                },
                MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 250);
            lf->Execute();

            return 1;
        },
        MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 750);
    lf->Execute();
}

void MapMaking::EmptyToEmpty(int i, int j, std::vector<std::vector<bool>> &visit)
{
    visit[i][j] = true;
    for (int u = -1; u <= 1; u++)
    {
        for (int v = -1; v <= 1; v++)
        {
            if (u * v)
                continue;
            int next_i = i + u;
            int next_j = j + v;
            if (!InRange(next_i, 1, Screen::map_size - 2) || !InRange(next_j, 1, Screen::map_size - 2))
                continue;
            if (!visit[next_i][next_j] && TileMap[next_i][next_j].first == EMPTY)
                EmptyToEmpty(next_i, next_j, visit);
        }
    }
}

bool MapMaking::Validation(int ei, int ej)
{
    std::vector<std::vector<bool>> visit(Screen::map_size, std::vector<bool>(Screen::map_size, false));
    EmptyToEmpty(ei, ej, visit);
    int countEmpty = 0;
    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            if (visit[i][j])
                countEmpty++;
        }
    }
    if (countEmpty < (Screen::map_size - 2) * (Screen::map_size - 2) - MapTile::nEmptyTiles)
        return false;
    return true;
}

void MapHUD()
{
    Canvas *hcv1 = new Canvas("horizontalhub1", Vector2(), Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    Button *clear = new Button(
        "clear", "Clear", Vector2(Screen::tile_size * 2, 0),
        []() {
            MapTile::DeleteTiles();
            MapMaking::drawSpawn = MapMaking::drawWin = false;
        },
        25);
    Button *save = new Button(
        "save", "Save", Vector2(Screen::tile_size * 2, 0), []() {}, 25);
    Button *random = new Button("random", "Random", Vector2(Screen::tile_size * 2, 0), MapMaking::Random, 25);
    hcv1->AddComponents({"clear", "save", "random"});

    Canvas *hcv2 = new Canvas("horizontalhub2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                              Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    Button *erase = new Button(
        "erase", "Erase", Vector2(Screen::tile_size * 2, 0), []() { MapMaking::currentDrawingType = EMPTY; }, 25);
    Button *drawWall = new Button(
        "wall", "Wall", Vector2(Screen::tile_size * 2, 0), []() { MapMaking::currentDrawingType = WALL; }, 25);
    Button *drawCoin = new Button(
        "coin", "Coin", Vector2(Screen::tile_size * 2, 0), []() { MapMaking::currentDrawingType = COIN; }, 25);
    Button *drawSpawn = new Button(
        "spawn", "Spawn", Vector2(Screen::tile_size * 2, 0), []() { MapMaking::currentDrawingType = SPAWN; }, 25);
    Button *drawWin = new Button(
        "win", "Win", Vector2(Screen::tile_size * 2, 0), []() { MapMaking::currentDrawingType = WIN; }, 25);
    hcv2->AddComponents({"erase", "wall", "coin", "spawn", "win"});
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