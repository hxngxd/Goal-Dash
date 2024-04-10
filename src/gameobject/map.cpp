#include "../datalib/PerlinNoise/PerlinNoise.hpp"
#include "../datalib/sprite.h"
#include "../event/input.h"
#include "../event/ui.h"
#include "../game.h"
#include <fstream>
#include <queue>
#include <set>

std::vector<std::vector<std::pair<int, Tile *>>> Map::Tiles;
Vector2 Map::spawn_tile, Map::win_tile;
bool Map::mode;
int Map::nempty = 0;
int Map::current_map;
std::map<int, int> Map::count_types;

Vector2 MapMaking::current_mouse_tile(-1);
int MapMaking::current_drawing_type = -1;

Tile::Tile(Vector2 position, Vector2 size)
{
    this->position = position;
    this->size = size;
    this->current_frame = 0;
    this->animation_delay = 0;
    this->animation_speed = 10;
    this->scale = 0;
}

void Map::Border()
{
    int &size = Screen::map_size;
    Tiles.clear();
    Tiles.resize(size, std::vector<std::pair<int, Tile *>>(size, std::make_pair(0, nullptr)));

    float wait = 500;

    for (int i = 0; i < size; i++)
    {
        Tiles[0][i].first = WALL;
        AddTile(0, i, wait);
    }

    for (int i = 1; i < size; i++)
    {
        Tiles[i][size - 1].first = WALL;
        AddTile(i, size - 1, wait);
    }

    for (int i = size - 2; i >= 0; i--)
    {
        Tiles[size - 1][i].first = WALL;
        AddTile(size - 1, i, wait);
    }

    for (int i = size - 2; i > 0; i--)
    {
        Tiles[i][0].first = WALL;
        AddTile(i, 0, wait);
    }
}

void Map::LoadMap()
{
    std::ifstream in;
    in.open("map/" + str(current_map) + ".map");

    int k = 0;
    while (!in.good())
    {
        in.close();
        current_map++;
        if (++k >= 100)
        {
            current_map = 1;
            break;
        }
        in.open("map/" + str(current_map) + ".map");
    }

    nempty = 0;
    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            in >> Tiles[i][j].first;
            if (Tiles[i][j].first)
                nempty++;
        }
    }
    in.close();
}

void Map::AddTile(int i, int j, float &wait, bool animation)
{
    count_types[Tiles[i][j].first]++;

    if (!Tiles[i][j].first)
        return;

    Tiles[i][j].second = new Tile(Vector2(j * Screen::tile_size, i * Screen::tile_size), Screen::tile_size);

    if (animation)
    {
        LinkedFunction *lf =
            new LinkedFunction(std::bind(TransformValue<float>, &Tiles[i][j].second->scale,
                                         Game::properties["tile_scale"].f, Game::properties["tile_rescale_speed"].f),
                               wait);
        lf->Execute();
    }
    else
        Tiles[i][j].second->scale = Game::properties["tile_scale"].f;
    wait += Game::properties["map_delay"].f;
}

void Map::AddTiles(bool create_win)
{
    int mp_size = Screen::map_size;

    nEmptyTiles = 0;

    if (UIs["map"])
        UIs["map"]->label = "Map: " + str(currentMap);

    currentMap++;

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
                if (!create_win)
                    continue;
            }
            CreateATile(i, j, wait);
        }
    }

    CreateATile(SpawnTile.x, SpawnTile.y, wait);
    if (!create_win)
        TileMap[WinTile.x][WinTile.y].first = 0;
    numofSpawn = numofWin = 1;
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
    int spawn_i, spawn_j;
    for (int i = 0; i < Screen::map_size; i++)
    {
        for (int j = 0; j < Screen::map_size; j++)
        {
            if (!TileMap[i][j].first || !TileMap[i][j].second)
                continue;

            if (!TileMap[i][j].second->scale)
                continue;

            if (i == 0 || i == Screen::map_size - 1)
                continue;

            SDL_Rect rect =
                Rect::Rescale(TileMap[i][j].second->position, TileMap[i][j].second->size, TileMap[i][j].second->scale);

            switch (TileMap[i][j].first)
            {
            case WIN:
                Animate(TileMap[i][j].second, "win");
                break;
            case SPAWN:
                spawn_i = i;
                spawn_j = j;
                Animate(TileMap[i][j].second, "spawn");
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(Game::Properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                break;
            case COIN:
                Screen::SetDrawColor(Color::yellow(Game::Properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                Animate(TileMap[i][j].second, "coin");
                break;
            default:
                break;
            }
        }
    }

    if (isMakingMap && MapMaking::currentDrawingType != -1)
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
            case EMPTY:
                Screen::SetDrawColor(Color::red(64));
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
        if (mouseLeft && ((Button *)UIs["erase"])->enabled)
        {
            int mx = MapMaking::mouseTile.x;
            int my = MapMaking::mouseTile.y;
            float wait = 0;
            int &type = MapMaking::currentDrawingType;
            if (type)
            {
                if (MapMaking::mouseTile != Vector2(-1) && TileMap[my][mx].first == EMPTY)
                {
                    if (type == SPAWN && !numofSpawn)
                    {
                        TileMap[my][mx].first = type;
                        CreateATile(my, mx, wait, 0);
                        numofSpawn++;
                    }
                    else if (type == WIN && !numofWin)
                    {
                        TileMap[my][mx].first = type;
                        CreateATile(my, mx, wait, 0);
                        numofWin++;
                    }
                    else if (type != SPAWN && type != WIN)
                    {
                        TileMap[my][mx].first = type;
                        CreateATile(my, mx, wait, 0);
                    }
                    MapTile::nEmptyTiles++;
                }
            }
            else
            {
                if (MapMaking::mouseTile != Vector2(-1) && TileMap[my][mx].first != EMPTY)
                {
                    if (TileMap[my][mx].first == SPAWN)
                        numofSpawn--;
                    else if (TileMap[my][mx].first == WIN)
                        numofWin--;
                    DeleteATile(my, mx, wait, 0);
                    MapTile::nEmptyTiles--;
                }
            }
        }
    }
}

std::vector<std::string> btns = {"clear", "save", "random", "prev", "next", "erase", "wall", "coin", "spawn", "win"};
void MapMaking::Random()
{
    print("generating map...");
    for (auto &btn : btns)
        ((Button *)UIs[btn])->enabled = false;

    numofSpawn = numofWin = 0;

    if (UIs["map"])
        UIs["map"]->label = "Map: 0";

    MapTile::DeleteTiles();
    LinkedFunction *lf = new LinkedFunction(
        []() {
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

            for (int j = 1; j < Screen::map_size - 1; j++)
            {
                int r1 = Screen::map_size - 2;
                int r2 = r1 - 1;
                if (TileMap[r1][j].first != WALL)
                    visitable[r1][j] = true;
                if (TileMap[r2][j].first != WALL)
                    visitable[r2][j] = true;
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
            DownVertical(spawn_i, spawn_j, visitable);
            MapTile::nEmptyTiles++;

            while (TileMap[spawn_i][spawn_j].first != WALL && spawn_i < Screen::map_size - 1)
                visitable[spawn_i++][spawn_j] = true;
            spawn_i--;

            // Generate coins
            for (float u = 1.0f; u <= 16.0f; u *= 2.0f)
            {
                for (float v = 1.0f; v <= 16.0f; v *= 2.0f)
                {
                    Trajectory(spawn_i, spawn_j, u, v, 0, visitable);
                    Trajectory(spawn_i, spawn_j, u, v, 1, visitable);
                }
            }
            Horizontal(spawn_i, spawn_j, 0, visitable);
            Horizontal(spawn_i, spawn_j, 1, visitable);

            for (int i = Screen::map_size - 2; i >= 1; i--)
            {
                for (int j = 1; j < Screen::map_size - 1; j++)
                {
                    if (visitable[i][j] && TileMap[i + 1][j].first == WALL)
                    {
                        for (float u = 1.0f; u <= 16.0f; u *= 2.0f)
                        {
                            for (float v = 1.0f; v <= 16.0f; v *= 2.0f)
                            {
                                Trajectory(i, j, u, v, 0, visitable);
                                Trajectory(i, j, u, v, 1, visitable);
                            }
                        }
                        Horizontal(i, j, 0, visitable);
                        Horizontal(i, j, 1, visitable);
                    }
                }
            }

            int CoinPossibility = 10;
            for (int i = 1; i < Screen::map_size - 1; i++)
            {
                for (int j = 1; j < Screen::map_size - 1; j++)
                {
                    if (visitable[i][j] && TileMap[i][j].first == EMPTY && RandomChoice(CoinPossibility))
                    {
                        TileMap[i][j].first = COIN;
                        MapTile::nEmptyTiles++;
                    }
                }
            }

            // Generate win
            int win_i = IntegralRandom<int>(1, 14);
            int win_j = IntegralRandom<int>(1, 14);
            while (TileMap[win_i][win_j].first != EMPTY || !visitable[win_i][win_j])
            {
                win_i = IntegralRandom<int>(1, 14);
                win_j = IntegralRandom<int>(1, 14);
            }
            TileMap[win_i][win_j].first = WIN;
            MapTile::nEmptyTiles++;

            numofSpawn = numofWin = 1;

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
                    for (auto &btn : btns)
                        ((Button *)UIs[btn])->enabled = true;
                    print("done");
                    return 1;
                },
                MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 250);
            lf->Execute();

            return 1;
        },
        MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 750);
    lf->Execute();
}

void MapMaking::Trajectory(int i, int j, float u, float v, bool isRight, std::vector<std::vector<bool>> &visitable)
{
    bool touched = false;
    Vector2 touched_pos(-1), touched_tile(-1);
    std::set<Vector2> tiles;

    Vector2 v0(Game::Properties["player_move_speed"].f / u, Game::Properties["player_jump_speed"].f / v);
    float g = Game::Properties["gravity"].f;

    Vector2 p0 = Vector2(j + 0.5f, i + 0.5f) * Screen::tile_size;
    Vector2 p, pt;

    float A = g / (2.0f * v0.x * v0.x);
    float B = v0.y / v0.x;
    float R = (2.0f * v0.x * v0.y) / g;
    float C = p0.x - (isRight ? 0 : R);
    float D;
    float h = (v0.y * v0.y) / (2.0f * g);

    float low = Screen::tile_size;
    float high = Game::Properties["resolution"].i - Screen::tile_size;

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
            float eps = 3.0f / v0.x;
            D = 2.0f * A * (p.x - C1) - B + eps;
        }

        bool up = (isRight == (D < 0));

        Vector2 current = Int(Vector2(p.x, p.y) / Screen::tile_size);
        if (!InRange(current.y, 0, Screen::map_size - 1))
            break;
        int &current_type = TileMap[current.y][current.x].first;
        if (current_type == WALL)
        {
            Vector2 tmp_p = current * Screen::tile_size;
            if (up && !touched)
            {
                touched = true;
                pt = p;
                bool a = tmp_p.y + Screen::tile_size * 0.5f <= pt.y && pt.y <= tmp_p.y + Screen::tile_size;
                float tmp_dy = tmp_p.y + Screen::tile_size - pt.y;
                bool b = tmp_p.x + tmp_dy <= pt.x && pt.x <= tmp_p.x + Screen::tile_size - tmp_dy;
                if (!a || !b)
                    return;
            }
            if (!up)
                return;
        }
        else
            visitable[current.y][current.x] = true;
    }
}

void MapMaking::Horizontal(int i, int j, bool isRight, std::vector<std::vector<bool>> &visitable)
{
    while (TileMap[i][j].first != WALL && (isRight ? j < Screen::map_size : j >= 0))
    {
        visitable[i][j] = true;
        if (TileMap[i + 1][j].first != WALL)
        {
            DownVertical(i + 1, j, visitable);
            break;
        }
        if (isRight)
            j++;
        else
            j--;
    }
}

void MapMaking::DownVertical(int i, int j, std::vector<std::vector<bool>> &visitable)
{
    while (TileMap[i][j].first != WALL && i < Screen::map_size)
    {
        visitable[i][j] = true;
        i++;
    }
    Horizontal(i - 1, j, 0, visitable);
    Horizontal(i - 1, j, 1, visitable);
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

void MapMaking::Save()
{
    if (!MapTile::nEmptyTiles)
    {
        print("nothing to save");
        return;
    }

    if (numofSpawn * numofWin != 1)
    {
        print("failed to save map");
        return;
    }

    int map = 1;
    std::ifstream in;
    in.open("map/" + str(map) + ".map");

    while (in.good())
    {
        in.close();
        map++;
        in.open("map/" + str(map) + ".map");
    }
    in.close();

    std::ofstream out;
    std::string filename = "map/" + str(map) + ".map";
    out.open(filename);

    if (!out.good())
    {
        print("failed to save map");
        return;
    }

    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            out << TileMap[i][j].first << " ";
        }
        if (i != Screen::map_size - 2)
            out << "\n";
    }
    out.close();

    if (UIs["map"])
        UIs["map"]->label = "Map: " + str(map);

    MapTile::currentMap = map;

    print("saved to", filename);
}

void MapMaking::ChangeMap()
{
    print("changing map...");
    for (auto &btn : btns)
        ((Button *)UIs[btn])->enabled = false;

    MapTile::DeleteTiles();
    LinkedFunction *lf = new LinkedFunction(
        []() {
            MapTile::CreateTiles(true);
            MapTile::currentMap--;
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    for (auto &btn : btns)
                        ((Button *)UIs[btn])->enabled = true;
                    print("done");
                    return 1;
                },
                MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 250);
            lf->Execute();

            return 1;
        },
        MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 750);
    lf->Execute();
}

void MapHUD()
{
    Canvas *hcv1 = new Canvas("horizontalhub1", Vector2(), Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    Button *clear = new Button(
        "clear", "Clear", Vector2(Screen::tile_size * 2, 0),
        []() {
            if (UIs["map"])
                UIs["map"]->label = "Map: 0";
            MapTile::currentMap = 0;
            print("clearing map...");
            for (auto &btn : btns)
                ((Button *)UIs[btn])->enabled = false;
            MapTile::DeleteTiles();
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    for (auto &btn : btns)
                        ((Button *)UIs[btn])->enabled = true;
                    numofSpawn = numofWin = MapTile::nEmptyTiles = 0;
                    print("done");
                    return 1;
                },
                MapTile::nEmptyTiles * Game::Properties["map_animation_delay"].f + 250);
            lf->Execute();
        },
        25);
    Button *save = new Button("save", "Save", Vector2(Screen::tile_size * 2, 0), MapMaking::Save, 25);
    Button *random = new Button("random", "Random", Vector2(Screen::tile_size * 2, 0), MapMaking::Random, 25);
    Button *prevmap = new Button(
        "prev", "<", Vector2(Screen::tile_size, 0),
        []() {
            if (MapTile::currentMap <= 1)
                return;
            MapTile::currentMap--;
            MapMaking::ChangeMap();
        },
        25);
    Button *nextmap = new Button(
        "next", ">", Vector2(Screen::tile_size, 0),
        []() {
            MapTile::currentMap++;
            MapMaking::ChangeMap();
        },
        25);
    Text *map = new Text("map", "Map: 0", Vector2(Screen::tile_size * 2, 0), 25);
    hcv1->AddComponents({"clear", "save", "random", "prev", "map", "next"});

    Canvas *hcv2 = new Canvas("horizontalhub2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                              Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    Button *erase = new Button(
        "erase", "Erase", Vector2(Screen::tile_size * 2, 0),
        []() { MapMaking::currentDrawingType = (MapMaking::currentDrawingType == EMPTY ? -1 : EMPTY); }, 25);
    Button *drawWall = new Button(
        "wall", "Wall", Vector2(Screen::tile_size * 2, 0),
        []() { MapMaking::currentDrawingType = (MapMaking::currentDrawingType == WALL ? -1 : WALL); }, 25);
    Button *drawCoin = new Button(
        "coin", "Coin", Vector2(Screen::tile_size * 2, 0),
        []() { MapMaking::currentDrawingType = (MapMaking::currentDrawingType == COIN ? -1 : COIN); }, 25);
    Button *drawSpawn = new Button(
        "spawn", "Spawn", Vector2(Screen::tile_size * 2, 0),
        []() { MapMaking::currentDrawingType = (MapMaking::currentDrawingType == SPAWN ? -1 : SPAWN); }, 25);
    Button *drawWin = new Button(
        "win", "Win", Vector2(Screen::tile_size * 2, 0),
        []() { MapMaking::currentDrawingType = (MapMaking::currentDrawingType == WIN ? -1 : WIN); }, 25);
    hcv2->AddComponents({"erase", "wall", "coin", "spawn", "win"});
}