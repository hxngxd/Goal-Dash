#include "datalib.h"
#include "game.h"
#include "gameobject.h"

std::vector<std::vector<std::pair<int, MapTile *>>> TileMap;
std::vector<Background> Backgrounds;

Vector2 MapTile::SpawnTile;
Vector2 MapTile::WinTile;

MapTile::MapTile(Vector2 position, Vector2 size, float wait)
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

    float wait = 0;

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

float MapTile::CreateTiles(int map)
{
    int mp_size = Screen::map_size;

    std::ifstream in;
    in.open("map/" + std::to_string(map) + ".map");

    // temporary code
    if (!in.good())
    {
        game->Stop();
        return {};
    }
    //

    for (int i = 1; i < mp_size - 1; i++)
    {
        for (int j = 1; j < mp_size - 1; j++)
        {
            in >> TileMap[i][j].first;
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
    return wait + Game::Properties["map_animation_delay"].f * 2;
}

void MapTile::CreateATile(int i, int j, float &wait)
{
    if (!TileMap[i][j].first)
        return;

    if (TileMap[i][j].first == COIN)
        Game::Properties["coin"].i++;

    TileMap[i][j].second = new MapTile(Vector2(j * Screen::tile_size, i * Screen::tile_size), Screen::tile_size, wait);

    wait += Game::Properties["map_animation_delay"].f;
    transformFValue(&TileMap[i][j].second->scale, 1, Game::Properties["rescale_speed"].f, wait);
}

float MapTile::DeleteTiles()
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
            transformFValue(&TileMap[i][j].second->scale, 0, Game::Properties["rescale_speed"].f, wait,
                            std::bind(post_func, i, j));
            wait += Game::Properties["map_animation_delay"].f;
        }
    }

    return wait;
}

void MapTile::Draw()
{
    for (int i = 0; i < Screen::map_size; i++)
    {
        for (int j = 0; j < Screen::map_size; j++)
        {
            if (!TileMap[i][j].first || !TileMap[i][j].second)
                continue;

            float currentTicks = SDL_GetTicks();
            if (!TileMap[i][j].second->scale)
                continue;

            SDL_Rect rect = Rect::reScale(TileMap[i][j].second->position, TileMap[i][j].second->size,
                                          TileMap[i][j].second->scale * 0.9);

            switch (TileMap[i][j].first)
            {
            case WIN:
                Screen::SetDrawColor(Color::green(255));
                break;
            case SPAWN:
                Screen::SetDrawColor(Color::cyan(255));
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(255));
                break;
            case COIN:
                if (currentTicks > TileMap[i][j].second->animation_delay + 1000 / TileMap[i][j].second->animation_speed)
                {
                    TileMap[i][j].second->currentFrame += 1;
                    if (TileMap[i][j].second->currentFrame >= Sprites["coin"]->maxFrames)
                        TileMap[i][j].second->currentFrame = 0;
                    TileMap[i][j].second->animation_delay = currentTicks;
                }
                Screen::SetDrawColor(Color::yellow(255));
                DrawSprite(Sprites["coin"], TileMap[i][j].second->position, TileMap[i][j].second->size,
                           TileMap[i][j].second->scale * 0.5, TileMap[i][j].second->currentFrame, 0);
                break;
            case DAMAGE:
                Screen::SetDrawColor(Color::red(255));
                break;
            default:
                break;
            }

            SDL_RenderDrawRect(Game::renderer, &rect);
        }
    }
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
    if (!loadSprite(name, path, maxFrames, realSize))
        return 0;
    ShowMsg(2, normal, "creating background " + name);
    Backgrounds.push_back(Background(name, scale));
    ShowMsg(3, success, "done.");
    SDL_SetTextureBlendMode(Sprites[name]->texture, SDL_BLENDMODE_BLEND);
    return 1;
}

void Background::Draw()
{
    Background &bg0 = Backgrounds[0];
    Background &bg1 = Backgrounds[1];
    Background &bg2 = Backgrounds[2];

    bg0.opacity = 150;
    setTextureOpacity(Sprites[bg0.name]->texture, bg0.opacity);
    DrawSprite(Sprites[bg0.name], bg0.position, bg0.size, bg0.scale, bg0.maxFrames, 0);

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

    setTextureOpacity(Sprites[bg2.name]->texture, bg2.opacity);
    DrawSprite(Sprites[bg2.name], bg2.position, bg2.size, bg2.scale, bg2.maxFrames, 0);

    bg1.opacity = 256 - bg2.opacity;
    setTextureOpacity(Sprites[bg1.name]->texture, bg1.opacity);
    DrawSprite(Sprites[bg1.name], bg1.position, bg1.size, bg1.scale, bg1.maxFrames, 1);
}

void Background::Move(Vector2 velocity, int index, float ratio)
{
    Background &bg = Backgrounds[index];

    if (velocity.x)
    {
        float bound = (1 - bg.scale) * bg.size.x * 0.5;
        bg.position.x += velocity.x * ratio;
        bg.position.x = clamp(bg.position.x, bound, -bound);
    }

    if (velocity.y)
    {
        float bound = (1 - bg.scale) * bg.size.y * 0.5;
        bg.position.y += velocity.y * ratio;
        bg.position.y = clamp(bg.position.y, bound, -bound);
    }
}