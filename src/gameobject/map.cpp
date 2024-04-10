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
bool Map::mode = 0;
int Map::nempty = 0;
int Map::current_map;
std::map<int, int> Map::count_types;

Vector2 MapMaking::current_mouse_tile(-1);
int MapMaking::current_drawing_type = -1;
bool MapMaking::allow_drawing = false;

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
    if (!Tiles[i][j].first)
        return;

    count_types[Tiles[i][j].first]++;

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

void Map::AddTiles()
{
    float wait = 0.1;
    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            if (Tiles[i][j].first == SPAWN)
            {
                spawn_tile = Vector2(j, i);
                continue;
            }
            else if (Tiles[i][j].first == WIN)
            {
                win_tile = Vector2(j, i);
                if (!mode)
                    continue;
            }
            AddTile(i, j, wait);
        }
    }

    AddTile(spawn_tile.y, spawn_tile.x, wait);
    if (!mode)
        Tiles[win_tile.y][win_tile.x].first = 0;
}

void Map::RemoveTile(int i, int j, float &wait, bool animation)
{
    if (!Tiles[i][j].first)
        return;

    count_types[Tiles[i][j].first]--;

    if (!Tiles[i][j].second)
        return;

    if (animation)
    {
        LinkedFunction *lf = new LinkedFunction(std::bind(TransformValue<float>, &Tiles[i][j].second->scale, 0.0f,
                                                          Game::properties["tile_rescale_speed"].f),
                                                wait);
        lf->NextFunction(std::bind(
            [](int i, int j) {
                Tiles[i][j].first = 0;
                if (Tiles[i][j].second)
                {
                    delete Tiles[i][j].second;
                    Tiles[i][j].second = nullptr;
                }
                return 1;
            },
            i, j));
        lf->Execute();
    }
    else
    {
        Tiles[i][j].first = 0;
        if (Tiles[i][j].second)
        {
            delete Tiles[i][j].second;
            Tiles[i][j].second = nullptr;
        }
    }
    wait += Game::properties["map_delay"].f;
}

void Map::RemoveTiles()
{
    float wait = 0.1;
    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            RemoveTile(i, j, wait);
        }
    }
}

void Map::Update()
{
    for (int i = 0; i < Screen::map_size; i++)
    {
        for (int j = 0; j < Screen::map_size; j++)
        {
            if (!Tiles[i][j].first || !Tiles[i][j].second)
                continue;

            if (!Tiles[i][j].second->scale)
                continue;

            SDL_Rect rect =
                Rect::Rescale(Tiles[i][j].second->position, Tiles[i][j].second->size, Tiles[i][j].second->scale);

            switch (Tiles[i][j].first)
            {
            case WIN:
                Animate(Tiles[i][j].second, "win");
                break;
            case SPAWN:
                Animate(Tiles[i][j].second, "spawn");
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(Game::properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                break;
            case COIN:
                Screen::SetDrawColor(Color::yellow(Game::properties["ray_opacity"].i));
                SDL_RenderDrawRect(Game::renderer, &rect);
                Animate(Tiles[i][j].second, "coin");
                break;
            default:
                break;
            }
        }
    }
}

void MapMaking::Update()
{
    if (MapMaking::current_drawing_type != -1)
    {
        int &ts = Screen::tile_size;

        current_mouse_tile = Int(EventHandler::MousePosition / ts);

        if (InRange(current_mouse_tile, Vector2(1), Vector2(Screen::map_size - 2)))
        {
            SDL_Rect mouseRect = {current_mouse_tile.x * ts, current_mouse_tile.y * ts, ts, ts};

            switch (MapMaking::current_drawing_type)
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
            current_mouse_tile = Vector2(-1);

        if (EventHandler::isMouseLeft && allow_drawing)
        {
            int mj = current_mouse_tile.x;
            int mi = current_mouse_tile.y;
            float wait = 0;
            if (current_drawing_type != EMPTY)
            {
                if (mi != -1 && Map::Tiles[mi][mj].first == EMPTY)
                {
                    if (current_drawing_type == SPAWN)
                    {
                        if (!Map::count_types[SPAWN])
                        {
                            Map::Tiles[mi][mj].first = current_drawing_type;
                            Map::AddTile(mi, mj, wait, 0);
                        }
                    }
                    else if (current_drawing_type == WIN)
                    {
                        if (!Map::count_types[WIN])
                        {
                            Map::Tiles[mi][mj].first = current_drawing_type;
                            Map::AddTile(mi, mj, wait, 0);
                        }
                    }
                    else
                    {
                        Map::Tiles[mi][mj].first = current_drawing_type;
                        Map::AddTile(mi, mj, wait, 0);
                    }
                    Map::nempty++;
                }
            }
            else
            {
                if (mi != -1 && Map::Tiles[mi][mj].first != EMPTY)
                {
                    Map::RemoveTile(mi, mj, wait, 0);
                    Map::nempty--;
                }
            }
        }
    }
}

std::vector<std::string> btns = {"clear", "save", "random", "prev", "next", "erase", "wall", "coin", "spawn", "win"};
void MapMaking::Random()
{
    print("generating map...");

    LinkedFunction *lf = new LinkedFunction(
        []() {
            std::vector<std::vector<bool>> visitable(Screen::map_size, std::vector<bool>(Screen::map_size, false));

            // Generate walls
            bool valid = false;
            while (!valid)
            {
                Map::nempty = 0;

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
                            Map::Tiles[i][j].first = WALL;
                            Map::nempty++;
                        }
                        else
                        {
                            Map::Tiles[i][j].first = EMPTY;
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
                if (Map::Tiles[r1][j].first != WALL)
                    visitable[r1][j] = true;
                if (Map::Tiles[r2][j].first != WALL)
                    visitable[r2][j] = true;
            }

            // Generate spawn
            int spawn_i = IntegralRandom<int>(1, 14);
            int spawn_j = IntegralRandom<int>(1, 14);
            while (Map::Tiles[spawn_i][spawn_j].first != EMPTY)
            {
                spawn_i = IntegralRandom<int>(1, 14);
                spawn_j = IntegralRandom<int>(1, 14);
            }
            Map::Tiles[spawn_i][spawn_j].first = SPAWN;
            DownVertical(spawn_i, spawn_j, visitable);
            Map::nempty++;

            while (Map::Tiles[spawn_i][spawn_j].first != WALL && spawn_i < Screen::map_size - 1)
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
                    if (visitable[i][j] && Map::Tiles[i + 1][j].first == WALL)
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
                    if (visitable[i][j] && Map::Tiles[i][j].first == EMPTY && RandomChoice(CoinPossibility))
                    {
                        Map::Tiles[i][j].first = COIN;
                        Map::nempty++;
                    }
                }
            }

            // Generate win
            int win_i = IntegralRandom<int>(1, 14);
            int win_j = IntegralRandom<int>(1, 14);
            while (Map::Tiles[win_i][win_j].first != EMPTY || !visitable[win_i][win_j])
            {
                win_i = IntegralRandom<int>(1, 14);
                win_j = IntegralRandom<int>(1, 14);
            }
            Map::Tiles[win_i][win_j].first = WIN;
            Map::nempty++;

            Map::AddTiles();

            LinkedFunction *lf = new LinkedFunction(
                []() {
                    allow_drawing = true;
                    for (auto &btn : btns)
                        ((Button *)UI::UIs[btn])->enabled = true;
                    print("done");
                    return 1;
                },
                Map::nempty * Game::properties["map_delay"].f + 250);
            lf->Execute();

            return 1;
        },
        Map::nempty * Game::properties["map_delay"].f + 750);

    Clear(lf);
}

void MapMaking::Trajectory(int i, int j, float u, float v, bool isRight, std::vector<std::vector<bool>> &visitable)
{
    bool touched = false;
    Vector2 touched_pos(-1), touched_tile(-1);
    std::set<Vector2> tiles;

    Vector2 v0(Game::properties["player_move_speed"].f / u, Game::properties["player_jump_speed"].f / v);
    float g = Game::properties["gravity"].f;

    Vector2 p0 = Vector2(j + 0.5f, i + 0.5f) * Screen::tile_size;
    Vector2 p, pt;

    float A = g / (2.0f * v0.x * v0.x);
    float B = v0.y / v0.x;
    float R = (2.0f * v0.x * v0.y) / g;
    float C = p0.x - (isRight ? 0 : R);
    float D;
    float h = (v0.y * v0.y) / (2.0f * g);

    float low = Screen::tile_size;
    float high = Game::properties["resolution"].i - Screen::tile_size;

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
        int &current_type = Map::Tiles[current.y][current.x].first;
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
    while (Map::Tiles[i][j].first != WALL && (isRight ? j < Screen::map_size : j >= 0))
    {
        visitable[i][j] = true;
        if (Map::Tiles[i + 1][j].first != WALL)
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
    while (Map::Tiles[i][j].first != WALL && i < Screen::map_size)
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
            if (!visit[next_i][next_j] && Map::Tiles[next_i][next_j].first == EMPTY)
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
    if (countEmpty < (Screen::map_size - 2) * (Screen::map_size - 2) - Map::nempty)
        return false;
    return true;
}

void MapMaking::Save()
{
    if (!Map::nempty)
    {
        print("nothing to save");
        return;
    }

    if (Map::count_types[SPAWN] != 1 || Map::count_types[WIN] != 1)
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
            out << Map::Tiles[i][j].first << " ";
        }
        if (i != Screen::map_size - 2)
            out << "\n";
    }
    out.close();

    Map::current_map = map;

    print("saved to", filename);
}

void MapMaking::ChangeMap()
{
    print("changing map...");
    LinkedFunction *lf = new LinkedFunction(
        []() {
            Map::LoadMap();
            Map::AddTiles();
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    allow_drawing = true;
                    for (auto &btn : btns)
                        ((Button *)UI::UIs[btn])->enabled = true;
                    print("done");
                    return 1;
                },
                Map::nempty * Game::properties["map_delay"].f + 250);
            lf->Execute();

            return 1;
        },
        Map::nempty * Game::properties["map_delay"].f + 750);
    Clear(lf);
}

void MapMaking::Clear(LinkedFunction *post_func)
{
    print("clearing map...");
    allow_drawing = false;
    for (auto &btn : btns)
        ((Button *)UI::UIs[btn])->enabled = false;
    Map::RemoveTiles();
    post_func->Execute();
}

void MapHUD()
{
    // Canvas *hcv1 = new Canvas("horizontalhub1", Vector2(), Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    // Button *clear = new Button(
    //     "clear", "Clear", Vector2(Screen::tile_size * 2, 0),
    //     []() {
    //         Map::current_map = 0;
    //         LinkedFunction *lf = new LinkedFunction(
    //             []() {
    //                 MapMaking::allow_drawing = true;
    //                 for (auto &btn : btns)
    //                     ((Button *)UI::UIs[btn])->enabled = true;
    //                 print("done");
    //                 return 1;
    //             },
    //             Map::nempty * Game::properties["map_delay"].f + 250);
    //         MapMaking::Clear(lf);
    //     },
    //     25);
    // Button *save = new Button("save", "Save", Vector2(Screen::tile_size * 2, 0), MapMaking::Save, 25);
    // Button *random = new Button("random", "Random", Vector2(Screen::tile_size * 2, 0), MapMaking::Random, 25);
    // Button *prevmap = new Button(
    //     "prev", "<", Vector2(Screen::tile_size, 0),
    //     []() {
    //         if (Map::current_map <= 1)
    //             return;
    //         Map::current_map--;
    //         MapMaking::ChangeMap();
    //     },
    //     25);
    // Button *nextmap = new Button(
    //     "next", ">", Vector2(Screen::tile_size, 0),
    //     []() {
    //         Map::current_map++;
    //         MapMaking::ChangeMap();
    //     },
    //     25);
    // Text *map = new Text("map", "Map: 0", Vector2(Screen::tile_size * 2, 0), 25);
    // hcv1->AddComponents({"clear", "save", "random", "prev", "map", "next"});

    // Canvas *hcv2 = new Canvas("horizontalhub2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
    //                           Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    // Button *erase = new Button(
    //     "erase", "Erase", Vector2(Screen::tile_size * 2, 0),
    //     []() { MapMaking::current_drawing_type = (MapMaking::current_drawing_type == EMPTY ? -1 : EMPTY); }, 25);
    // Button *drawWall = new Button(
    //     "wall", "Wall", Vector2(Screen::tile_size * 2, 0),
    //     []() { MapMaking::current_drawing_type = (MapMaking::current_drawing_type == WALL ? -1 : WALL); }, 25);
    // Button *drawCoin = new Button(
    //     "coin", "Coin", Vector2(Screen::tile_size * 2, 0),
    //     []() { MapMaking::current_drawing_type = (MapMaking::current_drawing_type == COIN ? -1 : COIN); }, 25);
    // Button *drawSpawn = new Button(
    //     "spawn", "Spawn", Vector2(Screen::tile_size * 2, 0),
    //     []() { MapMaking::current_drawing_type = (MapMaking::current_drawing_type == SPAWN ? -1 : SPAWN); }, 25);
    // Button *drawWin = new Button(
    //     "win", "Win", Vector2(Screen::tile_size * 2, 0),
    //     []() { MapMaking::current_drawing_type = (MapMaking::current_drawing_type == WIN ? -1 : WIN); }, 25);
    // hcv2->AddComponents({"erase", "wall", "coin", "spawn", "win"});
}