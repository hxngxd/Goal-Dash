#include "../datalib/PerlinNoise/PerlinNoise.hpp"
#include "../datalib/mixer.h"
#include "../datalib/sprite.h"
#include "../event/input.h"
#include "../event/ui.h"
#include "../game.h"
#include "filesystem"
#include <fstream>
#include <queue>
#include <set>

std::vector<std::vector<std::pair<int, Tile *>>> Map::Tiles;
Vector2 Map::spawn_tile, Map::win_tile;
int Map::mode = -1;
int Map::nempty = 0;
std::map<int, int> Map::count_types;

Vector2 MapMaking::current_mouse_tile(-1);
int MapMaking::current_drawing_type = -1;
bool MapMaking::allow_drawing = false;

std::vector<std::pair<std::string, std::string>> Map::MapPlaylist;
int Map::current_map_id;

Tile::Tile(Vector2 position, Vector2 size)
{
    this->position = position;
    this->size = size;
    this->current_frame = 0;
    this->animation_delay = 0;
    this->animation_speed = 8;
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

void Map::LoadMap(std::string path)
{
    print("loading map...");
    std::ifstream in;
    in.open(path);
    if (!in.good())
    {
        in.close();
        return;
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
    print("done");
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
    print("creating tiles...");
    float wait = 0.1;
    for (int i = 1; i < Screen::map_size - 1; i++)
    {
        for (int j = 1; j < Screen::map_size - 1; j++)
        {
            if (Tiles[i][j].first == SPAWN)
            {
                spawn_tile = Vector2(j, i);
                if (mode == 0)
                    continue;
            }
            else if (Tiles[i][j].first == WIN)
            {
                win_tile = Vector2(j, i);
                if (mode == 0)
                    continue;
            }
            AddTile(i, j, wait);
        }
    }

    if (mode == 0)
    {
        AddTile(spawn_tile.y, spawn_tile.x, wait);
        Tiles[win_tile.y][win_tile.x].first = 0;
    }

    print("done");
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
                Screen::SetDrawColor(Color::green(Game::properties["ray_opacity"].i));
                Animate(Tiles[i][j].second, "win");
                break;
            case SPAWN:
                Screen::SetDrawColor(Color::cyan(Game::properties["ray_opacity"].i));
                Animate(Tiles[i][j].second, "spawn");
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(Game::properties["ray_opacity"].i));
                break;
            case COIN:
                Screen::SetDrawColor(Color::yellow(Game::properties["ray_opacity"].i));
                Animate(Tiles[i][j].second, "coin");
                break;
            case HEALTH:
                Screen::SetDrawColor(Color::blue(Game::properties["ray_opacity"].i));
                Animate(Tiles[i][j].second, "health");
            default:
                break;
            }
            SDL_RenderDrawRect(Game::renderer, &rect);
        }
    }
}

Uint32 Map::GetMapDelay(Uint32 extend)
{
    return Map::nempty * Game::properties["map_delay"].f + extend;
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
                Screen::SetDrawColor(Color::cyan(64));
                break;
            case WALL:
                Screen::SetDrawColor(Color::white(64));
                break;
            case COIN:
                Screen::SetDrawColor(Color::yellow(64));
                break;
            case HEALTH:
                Screen::SetDrawColor(Color::blue(64));
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

        auto f = []() {
            Map::nempty++;
            if (Game::properties["sound"].b)
            {
                Mix_Volume(CHANNEL_RUN_BUILD, IntegralRandom<int>(1, 127));
                PlaySound("build", CHANNEL_RUN_BUILD, 0);
            }
        };

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
                            f();
                        }
                    }
                    else if (current_drawing_type == WIN)
                    {
                        if (!Map::count_types[WIN])
                        {
                            Map::Tiles[mi][mj].first = current_drawing_type;
                            Map::AddTile(mi, mj, wait, 0);
                            f();
                        }
                    }
                    else
                    {
                        Map::Tiles[mi][mj].first = current_drawing_type;
                        Map::AddTile(mi, mj, wait, 0);
                        f();
                    }
                }
            }
            else
            {
                if (mi != -1 && Map::Tiles[mi][mj].first != EMPTY)
                {
                    Map::RemoveTile(mi, mj, wait, 0);
                    Map::nempty--;
                    if (Game::properties["sound"].b)
                    {
                        Mix_Volume(CHANNEL_RUN_BUILD, IntegralRandom<int>(1, 127));
                        PlaySound("build", CHANNEL_RUN_BUILD, 0);
                    }
                }
            }
        }
    }
}

void MapMaking::Random()
{
    print("generating map...");

    Map::MapPlaylist.clear();

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
                        if (Game::properties["wall_possibility"].i)
                        {
                            float v = perlin.octave2D_01(j, i, 8) * 100.0f;
                            if (v > 90 - Game::properties["wall_possibility"].i)
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

            // visitable check
            while (Map::Tiles[spawn_i][spawn_j].first != WALL && spawn_i < Screen::map_size - 1)
                visitable[spawn_i++][spawn_j] = true;
            spawn_i--;

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

            // Generate coin
            for (int i = 1; i < Screen::map_size - 1; i++)
            {
                for (int j = 1; j < Screen::map_size - 1; j++)
                {
                    if (visitable[i][j] && Map::Tiles[i][j].first == EMPTY &&
                        RandomChoice(Game::properties["coin_possibility"].i))
                    {
                        Map::Tiles[i][j].first = COIN;
                        Map::nempty++;
                    }
                }
            }

            // Generate health
            if (Game::properties["health_possibility"].i)
            {
                for (int i = 1; i < Screen::map_size - 1; i++)
                {
                    for (int j = 1; j < Screen::map_size - 1; j++)
                    {
                        if (visitable[i][j] && Map::Tiles[i][j].first == EMPTY &&
                            RandomChoice(Game::properties["health_possibility"].i))
                        {
                            Map::Tiles[i][j].first = HEALTH;
                            Map::nempty++;
                        }
                    }
                }
            }

            Map::AddTiles();

            Text::SetLabel("MapBuilding-0.curmap", "Random map");
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    allow_drawing = true;
                    ToggleBtns(true);
                    print("done");
                    return 1;
                },
                Map::GetMapDelay());
            lf->Execute();

            return 1;
        },
        Map::GetMapDelay());

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
    float high = Screen::resolution.x - Screen::tile_size;

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

bool MapMaking::Save(bool as, std::string filename)
{
    if (!Map::nempty)
    {
        print("nothing to save");
        return 0;
    }

    if (Map::count_types[SPAWN] != 1 || Map::count_types[WIN] != 1)
    {
        print("failed to save map (not enough SPAWN or WIN)");
        return 0;
    }

    std::ofstream out;
    if (!as)
        out.open(Map::MapPlaylist.front().second);
    else
        out.open("map\\" + filename + ".map");

    if (!out.good())
    {
        print("failed to save map (can't open file)");
        out.close();
        return 0;
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

    if (!as)
        print("saved to", Map::MapPlaylist.front().second);
    else
    {
        print("saved to", filename);
        Map::MapPlaylist.clear();
        Map::MapPlaylist.push_back(std::make_pair("", "map\\" + filename + ".map"));
    }

    return 1;
}

void MapMaking::ChangeMap(std::string path)
{
    print("changing map...");
    LinkedFunction *lf = new LinkedFunction(std::bind(
                                                [](std::string path) {
                                                    Map::LoadMap(path);
                                                    Map::AddTiles();
                                                    LinkedFunction *lf = new LinkedFunction(
                                                        []() {
                                                            allow_drawing = true;
                                                            ToggleBtns(true);
                                                            print("done");
                                                            return 1;
                                                        },
                                                        Map::GetMapDelay());
                                                    lf->Execute();
                                                    return 1;
                                                },
                                                path),
                                            Map::GetMapDelay());
    Clear(lf);
}

bool MapMaking::Delete()
{
    if (Map::MapPlaylist.size() == 0)
        return 0;

    try
    {
        if (std::filesystem::remove(Map::MapPlaylist.front().second))
        {
            print(Map::MapPlaylist.front().second, "deleted");
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    MapMaking::allow_drawing = true;
                    MapMaking::ToggleBtns(true);
                    print("done");
                    return 1;
                },
                Map::GetMapDelay());
            Clear(lf);
            Text::SetLabel("MapBuilding-0.curmap", "No map selected");
            Map::MapPlaylist.clear();
            return 1;
        }
        else
        {
            print("failed to delete", Map::MapPlaylist.front().second);
            return 0;
        }
    }
    catch (const std::filesystem::filesystem_error &err)
    {
        print("error", err.what());
        return 0;
    }

    return 1;
}

void MapMaking::Clear(LinkedFunction *post_func)
{
    print("clearing map...");
    Text::SetLabel("MapBuilding-0.curmap", "No map selected");
    if (Map::mode == 1)
    {
        Map::MapPlaylist.clear();
        allow_drawing = false;
        ToggleBtns(false);
    }
    Map::RemoveTiles();
    Map::nempty = 0;
    if (post_func)
        post_func->Execute();
}

void MapMaking::ToggleBtns(bool toggle)
{
    std::vector<std::string> btns = {
        "MapBuilding-0.clear",  "MapBuilding-0.save",  "MapBuilding-0.random", "MapBuilding-0.delete",
        "MapBuilding-0.curmap", "MapBuilding-1.erase", "MapBuilding-1.wall",   "MapBuilding-1.coin",
        "MapBuilding-1.spawn",  "MapBuilding-1.win",   "Common.home",          "Common.settings",
    };
    for (auto &btn : btns)
    {
        if (UI::UIs.find(btn) != UI::UIs.end())
            ((Button *)UI::UIs[btn])->enabled = toggle;
    }
}