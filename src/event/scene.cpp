#include "scene.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../game.h"
#include "../gameobject/gameobject.h"
#include "../gameobject/map.h"
#include "ui.h"

void Scene::Welcome()
{
    print("creating welcome scene...");
    if (Map::Tiles.empty())
    {
        print("creating border...");
        Map::Border();
        print("border created...");
    }

    print("creating welcoming canvas...");
    Canvas *cv = new Canvas(
        "welcome", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 0, 8, 8, 1);

    print("creating title...");
    Text *title = new Text("title", Vector2(), Vector2(), "GOAL DASH", 55);

    print("creating buttons...");
    Button *start = new Button("start", Vector2(), Vector2(), "Start", Play, 50);
    Button *map = new Button("map", Vector2(), Vector2(), "Map Building", MapMaking, 50);

    Canvas *cv1 = new Canvas("welcome-1", Vector2(), Vector2(), 0, 8, 0, 0);

    Button *settings = new Button(
        "settings", Vector2(), Vector2(), "Settings", []() {}, 50);
    Button *about = new Button(
        "about", Vector2(), Vector2(), "About", []() { SDL_OpenURL("https://github.com/hxngxd"); }, 50);

    Button *exit = new Button(
        "exit", Vector2(), Vector2(), "Exit",
        []() {
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    game->running = false;
                    return 1;
                },
                100);
            lf->Execute();
        },
        50);

    cv->AddComponents({
        {"title", 2},
        {"start", 1},
        {"map", 1},
        {"welcome-1", 1},
        {"exit", 1},
    });

    cv1->AddComponents({
        {"settings", 1},
        {"about", 1},
    });

    print("done");
}

void Scene::Play()
{
    print("DB19");
    print("entering play mode...");
    Map::mode = 0;
    UI::DeleteUIs();
    Map::current_map == Game::properties["map_init"].i;
    print("DB20");
    Map::LoadMap();
    Map::AddTiles();

    Canvas *cv1 = new Canvas("play-1", Vector2(), Vector2(Screen::tile_size * 15, Screen::tile_size), 0, 0, 0, 0);

    Text *score = new Text("score", Vector2(), Vector2(), "Score: 0", 25);
    Text *time = new Text("time", Vector2(), Vector2(), "Time: 00:00:00.000", 25);
    Text *map = new Text("map", Vector2(), Vector2(), "Map: 0", 25);
    Text *dif = new Text("dif", Vector2(), Vector2(), "Difficulty: idk", 25);
    cv1->AddComponents({
        {"score", 3},
        {"time", 4},
        {"map", 3},
        {"dif", 5},
    });

    Canvas *cv2 = new Canvas("play-2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                             Vector2(Screen::tile_size * 3, Screen::tile_size), 0, 0, 0, 0);
    Text *hp = new Text("hp", Vector2(), Vector2(), "Health: 100", 25);
    cv2->AddComponents({
        {"hp", 3},
    });

    print("DB21");

    Common();
}

void Scene::MapMaking()
{
    print("entering map building mode...");
    Map::mode = 1;
    MapMaking::allow_drawing = true;
    UI::DeleteUIs();
    Map::current_map = Game::properties["map_init"].i;
    Map::LoadMap();
    Map::AddTiles();

    Canvas *cv1 = new Canvas("map-1", Vector2(), Vector2(Screen::tile_size * 10, Screen::tile_size), 0, 0, 0, 0);

    Button *clear = new Button(
        "clear", Vector2(), Vector2(), "Clear",
        []() {
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    MapMaking::allow_drawing = true;
                    MapMaking::ToggleBtns(true);
                    print("done");
                    return 1;
                },
                Map::nempty * Game::properties["map_delay"].f + 250);
            MapMaking::Clear(lf);
        },
        25);
    Button *save = new Button("save", Vector2(), Vector2(), "Save", MapMaking::Save, 25);
    Button *random = new Button("random", Vector2(), Vector2(), "Random", MapMaking::Random, 25);

    Button *prev = new Button(
        "prev", Vector2(), Vector2(), "<",
        []() {
            if (Map::current_map <= 1)
                return;
            Map::current_map--;
            MapMaking::ChangeMap();
        },
        25);
    Text *map = new Text("map", Vector2(), Vector2(), "Map: 0", 25);
    Button *next = new Button(
        "next", Vector2(), Vector2(), ">",
        []() {
            Map::current_map++;
            MapMaking::ChangeMap();
        },
        25);

    cv1->AddComponents({
        {"clear", 2},
        {"save", 2},
        {"random", 2},
        {"prev", 1},
        {"map", 2},
        {"next", 1},
    });

    Canvas *cv2 = new Canvas("map-2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                             Vector2(Screen::tile_size * 10, Screen::tile_size), 0, 0, 0, 0);

    auto change_drawing_type = [](int type) {
        MapMaking::current_drawing_type = (MapMaking::current_drawing_type == type ? -1 : type);
    };
    Button *erase = new Button("erase", Vector2(), Vector2(), "EMPTY", std::bind(change_drawing_type, EMPTY), 25);
    Button *wall = new Button("wall", Vector2(), Vector2(), "WALL", std::bind(change_drawing_type, WALL), 25);
    Button *coin = new Button("coin", Vector2(), Vector2(), "COIN", std::bind(change_drawing_type, COIN), 25);
    Button *spawn = new Button("spawn", Vector2(), Vector2(), "SPAWN", std::bind(change_drawing_type, SPAWN), 25);
    Button *win = new Button("win", Vector2(), Vector2(), "WIN", std::bind(change_drawing_type, WIN), 25);

    cv2->AddComponents({
        {"erase", 2},
        {"wall", 2},
        {"coin", 2},
        {"spawn", 2},
        {"win", 2},
    });

    Common();
}

void Scene::Common()
{
    Canvas *cv = new Canvas("common", Vector2(Screen::tile_size * (Screen::map_size - 1), 0),
                            Vector2(Screen::tile_size, Screen::tile_size * 4), 0, 0, 0, 1);
    Button *exit = new Button(
        "exit", Vector2(), Vector2(), "Exit",
        []() {
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    game->running = false;
                    return 1;
                },
                100);
            lf->Execute();
        },
        25);

    Button *home = new Button(
        "home", Vector2(), Vector2(), "Home",
        []() {
            print("going back home...");
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    UI::DeleteUIs();
                    Scene::Welcome();
                    print("done");
                    return 1;
                },
                Map::nempty * Game::properties["map_delay"].f + 250);
            MapMaking::Clear(lf);
        },
        25);
    Button *settings = new Button(
        "settings", Vector2(), Vector2(), "Settings", []() {}, 25);
    Button *mute = new Button(
        "mute", Vector2(), Vector2(), "Mute",
        []() {
            bool &msc = Game::properties["music"].b;
            bool &snd = Game::properties["sound"].b;
            if (msc)
                Mix_PauseMusic();
            else
                Mix_ResumeMusic();
            if (snd)
                StopAllSound();
            msc = !msc;
            snd = !snd;
        },
        25);

    cv->AddComponents({
        {"exit", 1},
        {"home", 1},
        {"settings", 1},
        {"mute", 1},
    });
}

// Scene::Scene(bool create_player)
// {
//     print("creating new scene...");
//     print("creating tiles...");
//     MapTile::CreateTiles(!create_player);

//     if (!create_player)
//     {
//         return;
//     }
//     LinkedFunction *lf = new LinkedFunction(
//         std::bind([]() {
//             print("tiles created");
//             print("creating player...");
//             PlaySound("spawn", CHANNEL_SPAWN_WIN, 0);

//             Vector2 player_position(MapTile::SpawnTile.y * Screen::tile_size, MapTile::SpawnTile.x *
//             Screen::tile_size); Game::player = new Player(player_position); if (!Game::player_time[0])
//                 Game::player_time[0] = SDL_GetTicks();
//             Game::player_time[1] = SDL_GetTicks();
//             if (Game::player_time[2])
//                 Game::player_time[0] += (Game::player_time[1] - Game::player_time[2]);
//             return 1;
//         }),
//         (MapTile::nEmptyTiles + 4) * Game::Properties["map_delay"].f);
//     lf->NextFunction([]() { return TransformValue(&Game::player->scale, 1.0f,
//     Game::Properties["tile_rescale_speed"].f);
//     },
//                      0);
//     lf->NextFunction(
//         []() {
//             return TransformValue(&TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y].second->scale, 0.0f,
//                                   Game::Properties["tile_rescale_speed"].f);
//         },
//         500);
//     lf->NextFunction(
//         []() {
//             std::pair<int, MapTile *> &spawn_tile = TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y];
//             spawn_tile.first = 0;
//             delete spawn_tile.second;
//             spawn_tile.second = nullptr;
//             print("player created");
//             return 1;
//         },
//         100);
//     lf->NextFunction([]() {
//         print("scene created");
//         Game::player_won = false;
//         return 1;
//     });
//     lf->Execute();
// }