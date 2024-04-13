#include "scene.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../game.h"
#include "../gameobject/gameobject.h"
#include "../gameobject/map.h"
#include "ui.h"

Vector2 v;

void Scene::Welcome()
{
    print("creating welcome scene...");

    Canvas *canvas = new Canvas(
        "Welcome", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 128, 8, 8, 1);
    canvas->AddComponents({
        {new Text("title", v, v, "GOAL DASH", 1, 55), 2},
        {new Button("start", v, v, "Start", Play, 50), 1},
        {new Button("mapbuilding", v, v, "Map Building", MapMaking, 50), 1},
    });

    Canvas *canvas1 = new Canvas("Section-1", v, v, 0, 8, 0, 0);
    canvas1->AddComponents({
        {new Button(
             "settings", v, v, "Settings",
             []() {
                 UI::SetVisible("Welcome", false);
                 UI::SetVisible("Settings", true);
             },
             50),
         1},
        {new Button(
             "about", v, v, "About", []() { SDL_OpenURL("https://github.com/hxngxd"); }, 50),
         1},
    });

    canvas->AddComponents({
        {canvas1, 1},
        {new Button(
             "exit", v, v, "Exit",
             []() {
                 LinkedFunction *lf = new LinkedFunction(
                     []() {
                         game->running = false;
                         return 1;
                     },
                     100);
                 lf->Execute();
             },
             50),
         1},
    });

    if (Map::Tiles.empty())
    {
        print("creating border...");
        Map::Border();
        print("border created...");
    }

    print("done");
}

void Scene::Play()
{
    print("entering play mode...");
    Map::mode = 0;

    UI::RemovingUI("Welcome");

    MapMaking::ToggleBtns(false);

    Map::current_map == Game::properties["map_init"].i;
    Map::LoadMap();

    Canvas *canvas0 = new Canvas("Play-0", v, Vector2(Screen::tile_size * 15, Screen::tile_size), 255, 0, 0, 0);

    canvas0->AddComponents({
        {new Text("score", v, v, "Score: 0", 1, 25), 3},
        {new Text("time", v, v, "Time: 00:00:00.000", 1, 25), 4},
        {new Text("map", v, v, "Map: " + str(Map::current_map), 1, 25), 3},
        {new Text("dif", v, v, "Difficulty: idk", 1, 25), 5},
    });

    Canvas *canvas1 = new Canvas("Play-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                                 Vector2(Screen::tile_size * 3, Screen::tile_size), 255, 0, 0, 0);

    canvas1->AddComponents({
        {new Text("hp", v, v, "Health: 100", 1, 25), 3},
    });

    UI::SetVisible("Common", true);

    Game::time[0] = SDL_GetTicks();
    Game::time[1] = Game::time[2] = 0;

    SpawnPlayer();
    Map::AddTiles();

    LinkedFunction *lf = new LinkedFunction(
        []() {
            MapMaking::ToggleBtns(true);
            print("done");
            return 1;
        },
        Map::GetMapDelay());
    lf->Execute();

    print("done");
}

void Scene::MapMaking()
{
    print("entering map building mode...");
    Map::mode = 1;
    MapMaking::allow_drawing = true;
    MapMaking::ToggleBtns(false);
    UI::RemovingUI("Welcome");

    Map::current_map = Game::properties["map_init"].i;
    Map::LoadMap();

    Canvas *canvas0 = new Canvas("MapBuilding-0", v, Vector2(Screen::tile_size * 10, Screen::tile_size), 255, 0, 0, 0);

    canvas0->AddComponents({
        {new Button(
             "clear", v, v, "Clear",
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
             25),
         2},
        {new Button("save", v, v, "Save", MapMaking::Save, 25), 2},
        {new Button("random", v, v, "Random", MapMaking::Random, 25), 2},
        {new Button(
             "prev", v, v, "<",
             []() {
                 if (Map::current_map <= 1)
                     return;
                 Map::current_map--;
                 MapMaking::ChangeMap();
             },
             25),
         1},
        {new Text("curmap", v, v, "Map: " + str(Map::current_map), 1, 25), 2},
        {new Button(
             "next", v, v, ">",
             []() {
                 Map::current_map++;
                 MapMaking::ChangeMap();
             },
             25),
         1},
    });

    Canvas *canvas1 = new Canvas("MapBuilding-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                                 Vector2(Screen::tile_size * 10, Screen::tile_size), 255, 0, 0, 0);

    auto change_drawing_type = [](int type) {
        MapMaking::current_drawing_type = (MapMaking::current_drawing_type == type ? -1 : type);
    };

    canvas1->AddComponents({
        {new Button("erase", v, v, "EMPTY", std::bind(change_drawing_type, EMPTY), 25), 2},
        {new Button("wall", v, v, "WALL", std::bind(change_drawing_type, WALL), 25), 2},
        {new Button("coin", v, v, "COIN", std::bind(change_drawing_type, COIN), 25), 2},
        {new Button("spawn", v, v, "SPAWN", std::bind(change_drawing_type, SPAWN), 25), 2},
        {new Button("win", v, v, "WIN", std::bind(change_drawing_type, WIN), 25), 2},
    });

    UI::SetVisible("Common", true);

    Map::AddTiles();

    LinkedFunction *lf = new LinkedFunction(
        []() {
            MapMaking::ToggleBtns(true);
            print("done");
            return 1;
        },
        Map::GetMapDelay());
    lf->Execute();

    print("done");
}

void Scene::Common()
{
    Canvas *canvas0 = new Canvas("Common", Vector2(Screen::tile_size * (Screen::map_size - 1), 0),
                                 Vector2(Screen::tile_size, Screen::tile_size * 4), 255, 0, 0, 1);

    UI::SetVisible("Common", false);

    canvas0->AddComponents({
        {new Button(
             "exit", v, v, "Exit",
             []() {
                 LinkedFunction *lf = new LinkedFunction(
                     []() {
                         game->running = false;
                         return 1;
                     },
                     100);
                 lf->Execute();
             },
             25),
         1},
        {new Button(
             "home", v, v, "Home",
             []() {
                 print("going back home...");
                 auto goHome = []() {
                     LinkedFunction *lf = new LinkedFunction(
                         []() {
                             if (Map::mode == 0)
                             {
                                 UI::RemovingUI("Play-0");
                                 UI::RemovingUI("Play-1");
                             }
                             else if (Map::mode == 1)
                             {
                                 MapMaking::allow_drawing = false;
                                 MapMaking::current_drawing_type = -1;
                                 UI::RemovingUI("MapBuilding-0");
                                 UI::RemovingUI("MapBuilding-1");
                             }
                             UI::SetVisible("Common", false);
                             UI::SetVisible("Settings", false);
                             MapMaking::ToggleBtns(true);
                             Map::mode = -1;
                             Scene::Welcome();
                             print("done");
                             return 1;
                         },
                         Map::GetMapDelay());
                     MapMaking::Clear(lf);
                     return 1;
                 };
                 if (Map::mode == 0 && Game::player)
                 {
                     LinkedFunction *lf = new LinkedFunction(
                         []() {
                             return TransformValue(&Game::player->scale, 0.0f,
                                                   Game::properties["tile_rescale_speed"].f);
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
             25),
         1},
        {new Button(
             "settings", v, v, "Settings",
             []() {
                 UI::SetVisible("Settings", true);
                 switch (Map::mode)
                 {
                 case 0:
                     UI::SetVisible("Play-0", false);
                     UI::SetVisible("Play-1", false);
                     break;
                 case 1:
                     UI::SetVisible("MapBuilding-0", false);
                     UI::SetVisible("MapBuilding-1", false);
                     break;
                 }
             },
             25),
         1},
        {new Button(
             "mute", v, v, "Mute",
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
             25),
         1},
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

void Scene::Settings()
{
    int font_size = 21;

    Canvas *canvas0 = new Canvas("Settings", Vector2(Screen::tile_size),
                                 Screen::resolution - Vector2(Screen::tile_size * 2), 180, 0, 0, 1);

    UI::SetVisible("Settings", false);

    canvas0->AddComponent(new Text("title", v, v, "SETTINGS", 1, 2 * font_size), 2);

    std::vector<Canvas *> canvases = {canvas0};

    for (int i = 1; i <= 12; i++)
    {
        canvases.push_back(new Canvas("Second-" + str(i), v, v, 0, 0, 0, 0));
        canvas0->AddComponent(canvases[i]);
    }

    canvases[1]->AddComponents({
        {new Text("movespeed", v, v, "Move speed", 0, font_size), 1},
        {new Slider("movespeedslider", v, v, 0.5f, 2.5f, 1.0f, 0.1f, font_size), 3},
    });

    canvases[2]->AddComponents({{new Text("jumpspeed", v, v, "Jump speed", 0, font_size), 1},
                                {new Slider("jumpspeedslider", v, v, 0.5f, 2.5f, 1.0f, 0.1f, font_size), 3}});

    canvases[3]->AddComponents({{new Text("acceleration", v, v, "Acceleration", 0, font_size), 1},
                                {new Slider("accelerationslider", v, v, 0.25f, 2.0f, 1.0f, 0.1f, font_size), 3}});

    canvases[4]->AddComponents({
        {new Text("gravity", v, v, "Gravity", 0, font_size), 1},
        {new Slider("gravityslider", v, v, 0.0f, 10.0f, 5.0f, 0.1f, font_size), 3},
    });

    canvases[5]->AddComponents({
        {new Text("immortal", v, v, "Immortal", 1, font_size), 2},
        {new Toggle("immortaltoggle", v, v, false), 1},

        {new Text("collision", v, v, "Map collision", 1, font_size), 2},
        {new Toggle("collisiontoggle", v, v, true), 1},

        {new Text("keyboard", v, v, "Key layout", 1, font_size), 2},
        {new Toggle("keyboardtoggle", v, v, false), 1},
    });

    canvases[6]->AddComponents({
        {new Text("music", v, v, "Music", 1, font_size), 2},
        {new Toggle("musictoggle", v, v, false), 1},
        {new Text("sound", v, v, "SoundFX", 1, font_size), 2},
        {new Toggle("soundtoggle", v, v, false), 1},
    });

    canvases[7]->AddComponents({
        {new Text("volume", v, v, "Volume", 0, font_size), 1},
        {new Slider("volumeslider", v, v, 0.0f, 127.0f, 64.0f, 1.0f, font_size), 3},
    });

    canvases[8]->AddComponents({
        {new Text("background", v, v, "Background", 1, font_size), 2},
        {new Toggle("backgroundtoggle", v, v, true), 1},
        {new Text("grid", v, v, "Point grid", 1, font_size), 2},
        {new Toggle("gridtoggle", v, v, true), 1},
    });

    canvases[9]->AddComponents({
        {new Text("ray", v, v, "Ray opacity", 0, font_size), 1},
        {new Slider("rayslider", v, v, 0.0f, 255.0f, 180.0f, 0.1f, font_size), 3},
    });

    canvases[10]->AddComponents({
        {new Text("mapinit", v, v, "Starting map", 0, font_size), 4},
        {new Button(
             "prev", v, v, "<", []() {}, font_size),
         1},
        {new Text("curmap", v, v, "Map 1", 1, font_size), 2},
        {new Button(
             "next", v, v, ">", []() {}, font_size),
         1},
    });

    canvases[11]->AddComponents({
        {new Text("mapdelay", v, v, "Map animation delay", 0, font_size), 1},
        {new Slider("mapdelayslidere", v, v, 0.0f, 100.0f, 15.0f, 0.1f, font_size), 1},
    });

    canvases[12]->AddComponents({
        {new Button(
             "save", v, v, "Save", []() {}, font_size),
         1},
        {new Button(
             "default", v, v, "Default Settings", []() {}, font_size),
         1},
        {new Button(
             "exit", v, v, "Exit",
             []() {
                 UI::SetVisible("Settings", false);
                 switch (Map::mode)
                 {
                 case -1:
                     UI::SetVisible("Welcome", true);
                 case 0:
                     UI::SetVisible("Play-0", true);
                     UI::SetVisible("Play-1", true);
                     break;
                 case 1:
                     UI::SetVisible("MapBuilding-0", true);
                     UI::SetVisible("MapBuilding-1", true);
                     break;
                 }
             },
             font_size),
         1},
    });
}