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
    Canvas *cv0 =
        new Canvas("welcome-canvas-0",
                   Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
                   Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 0, 8, 8, 1);

    print("creating title...");
    Text *title = new Text("welcome-canvas-0-title", Vector2(), Vector2(), "GOAL DASH", 55);

    print("creating buttons...");
    Button *start = new Button("welcome-canvas-0-start", Vector2(), Vector2(), "Start", Play, 50);

    Button *map = new Button("welcome-canvas-0-map", Vector2(), Vector2(), "Map Building", MapMaking, 50);

    Canvas *cv1 = new Canvas("welcome-canvas-1", Vector2(), Vector2(), 0, 8, 0, 0);

    Button *settings = new Button(
        "welcome-canvas-1-settings", Vector2(), Vector2(), "Settings", []() {}, 50);

    Button *about = new Button(
        "welcome-canvas-1-about", Vector2(), Vector2(), "About", []() { SDL_OpenURL("https://github.com/hxngxd"); },
        50);

    Button *exit = new Button(
        "welcome-canvas-0-exit", Vector2(), Vector2(), "Exit",
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

    cv0->AddComponents({
        {"welcome-canvas-0-title", 2},
        {"welcome-canvas-0-start", 1},
        {"welcome-canvas-0-map", 1},
        {"welcome-canvas-1", 1},
        {"welcome-canvas-0-exit", 1},
    });

    cv1->AddComponents({
        {"welcome-canvas-1-settings", 1},
        {"welcome-canvas-1-about", 1},
    });

    print("done");
}

void Scene::Play()
{
    print("entering play mode...");
    Map::mode = 0;
    UI::RemovingUIs();
    Map::current_map == Game::properties["map_init"].i;
    Map::LoadMap();

    Canvas *cv0 =
        new Canvas("play-canvas-0", Vector2(), Vector2(Screen::tile_size * 15, Screen::tile_size), 0, 0, 0, 0);

    Text *score = new Text("play-canvas-0-score", Vector2(), Vector2(), "Score: 0", 25);

    Text *time = new Text("play-canvas-0-time", Vector2(), Vector2(), "Time: 00:00:00.000", 25);

    Text *map = new Text("play-canvas-0-map", Vector2(), Vector2(), "Map: " + str(Map::current_map), 25);

    Text *dif = new Text("play-canvas-0-dif", Vector2(), Vector2(), "Difficulty: idk", 25);

    cv0->AddComponents({
        {"play-canvas-0-score", 3},
        {"play-canvas-0-time", 4},
        {"play-canvas-0-map", 3},
        {"play-canvas-0-dif", 5},
    });

    Canvas *cv1 = new Canvas("play-canvas-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                             Vector2(Screen::tile_size * 3, Screen::tile_size), 0, 0, 0, 0);

    Text *hp = new Text("play-canvas-1-hp", Vector2(), Vector2(), "Health: 100", 25);

    cv1->AddComponents({
        {"play-canvas-1-hp", 3},
    });

    Common();

    Game::time[0] = SDL_GetTicks();
    Game::time[1] = Game::time[2] = 0;

    SpawnPlayer();
    Map::AddTiles();

    print("done");
}

void Scene::MapMaking()
{
    print("entering map building mode...");
    Map::mode = 1;
    MapMaking::allow_drawing = true;
    UI::RemovingUIs();
    Map::current_map = Game::properties["map_init"].i;
    Map::LoadMap();

    Canvas *cv0 = new Canvas("map-canvas-0", Vector2(), Vector2(Screen::tile_size * 10, Screen::tile_size), 0, 0, 0, 0);

    Button *clear = new Button(
        "map-canvas-0-clear", Vector2(), Vector2(), "Clear",
        []() {
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    MapMaking::allow_drawing = true;
                    MapMaking::ToggleBtns(true);
                    print("done");
                    return 1;
                },
                Map::GetMapDelay());
            MapMaking::Clear(lf);
        },
        25);

    Button *save = new Button("map-canvas-0-save", Vector2(), Vector2(), "Save", MapMaking::Save, 25);

    Button *random = new Button("map-canvas-0-random", Vector2(), Vector2(), "Random", MapMaking::Random, 25);

    Button *prev = new Button(
        "map-canvas-0-prev", Vector2(), Vector2(), "<",
        []() {
            if (Map::current_map <= 1)
                return;
            Map::current_map--;
            MapMaking::ChangeMap();
        },
        25);
    Text *map = new Text("map-canvas-0-map", Vector2(), Vector2(), "Map: " + str(Map::current_map), 25);

    Button *next = new Button(
        "map-canvas-0-next", Vector2(), Vector2(), ">",
        []() {
            Map::current_map++;
            MapMaking::ChangeMap();
        },
        25);

    cv0->AddComponents({
        {"map-canvas-0-clear", 2},
        {"map-canvas-0-save", 2},
        {"map-canvas-0-random", 2},
        {"map-canvas-0-prev", 1},
        {"map-canvas-0-map", 2},
        {"map-canvas-0-next", 1},
    });

    Canvas *cv1 = new Canvas("map-canvas-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                             Vector2(Screen::tile_size * 10, Screen::tile_size), 0, 0, 0, 0);

    auto change_drawing_type = [](int type) {
        MapMaking::current_drawing_type = (MapMaking::current_drawing_type == type ? -1 : type);
    };

    Button *erase =
        new Button("map-canvas-1-erase", Vector2(), Vector2(), "EMPTY", std::bind(change_drawing_type, EMPTY), 25);

    Button *wall =
        new Button("map-canvas-1-wall", Vector2(), Vector2(), "WALL", std::bind(change_drawing_type, WALL), 25);

    Button *coin =
        new Button("map-canvas-1-coin", Vector2(), Vector2(), "COIN", std::bind(change_drawing_type, COIN), 25);

    Button *spawn =
        new Button("map-canvas-1-spawn", Vector2(), Vector2(), "SPAWN", std::bind(change_drawing_type, SPAWN), 25);

    Button *win = new Button("map-canvas-1-win", Vector2(), Vector2(), "WIN", std::bind(change_drawing_type, WIN), 25);

    cv1->AddComponents({
        {"map-canvas-1-erase", 2},
        {"map-canvas-1-wall", 2},
        {"map-canvas-1-coin", 2},
        {"map-canvas-1-spawn", 2},
        {"map-canvas-1-win", 2},
    });

    Common();

    Map::AddTiles();

    print("done");
}

void Scene::Common()
{
    Canvas *cv0 = new Canvas("common-canvas-0", Vector2(Screen::tile_size * (Screen::map_size - 1), 0),
                             Vector2(Screen::tile_size, Screen::tile_size * 4), 0, 0, 0, 1);

    Button *exit = new Button(
        "common-canvas-0-exit", Vector2(), Vector2(), "Exit",
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
        "common-canvas-0-home", Vector2(), Vector2(), "Home",
        []() {
            print("going back home...");
            auto goHome = []() {
                LinkedFunction *lf = new LinkedFunction(
                    []() {
                        UI::RemovingUIs();
                        Scene::Welcome();
                        print("done");
                        return 1;
                    },
                    Map::GetMapDelay());
                MapMaking::Clear(lf);
                return 1;
            };
            if (!Map::mode && Game::player)
            {
                LinkedFunction *lf = new LinkedFunction(
                    []() {
                        return TransformValue(&Game::player->scale, 0.0f, Game::properties["tile_rescale_speed"].f);
                    },
                    250);
                lf->NextFunction([]() {
                    delete Game::player;
                    Game::player = nullptr;
                    return 1;
                });
                lf->NextFunction(goHome);
                lf->Execute();
            }
            else
                goHome();
        },
        25);

    Button *settings = new Button(
        "common-canvas-0-settings", Vector2(), Vector2(), "Settings", []() {}, 25);

    Button *mute = new Button(
        "common-canvas-0-mute", Vector2(), Vector2(), "Mute",
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

    cv0->AddComponents({
        {"common-canvas-0-exit", 1},
        {"common-canvas-0-home", 1},
        {"common-canvas-0-settings", 1},
        {"common-canvas-0-mute", 1},
    });
}

void Scene::SpawnPlayer()
{
    LinkedFunction *lf = new LinkedFunction(
        []() {
            print("creating player...");
            PlaySound("spawn", CHANNEL_SPAWN_WIN, 0);
            Game::player = new Player(Map::spawn_tile * Screen::tile_size);
            Game::time[2] = SDL_GetTicks();
            if (Game::time[1])
                Game::time[0] += Game::time[2] - Game::time[1];

            return 1;
        },
        Map::GetMapDelay(100));
    lf->NextFunction(
        []() { return TransformValue(&Game::player->scale, 1.0f, Game::properties["tile_rescale_speed"].f); }, 0);
    lf->NextFunction(
        []() {
            float wait = 0.1f;
            Map::RemoveTile(Map::spawn_tile.y, Map::spawn_tile.x, wait);
            print("done");
            return 1;
        },
        500);
    lf->Execute();
}