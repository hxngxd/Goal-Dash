#include "scene.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../game.h"
#include "../gameobject/gameobject.h"
#include "../gameobject/map.h"
#include "ui.h"
#include <filesystem>

Vector2 v;

int border_opacity = 32;

void Scene::Welcome()
{
    print("creating welcome scene...");

    Canvas *canvas = new Canvas(
        "Welcome", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 180, 8, 8, 1, border_opacity);
    canvas->AddComponents({
        {new Text("title", v, v, "GOAL DASH", 1, 2.5f * Screen::font_size, border_opacity), 2},
        {new Button("start", v, v, "Start", Play, 2.0f * Screen::font_size, border_opacity), 1},
        {new Button("mapbuilding", v, v, "Map Building", MapMaking, 2.0f * Screen::font_size, border_opacity), 1},
    });

    Canvas *canvas1 = new Canvas("Section-1", v, v, 0, 8, 0, 0);
    canvas1->AddComponents({
        {new Button(
             "settings", v, v, "Settings",
             []() {
                 UI::SetVisible("Welcome", false);
                 UI::SetVisible("Settings", true);
             },
             2.0f * Screen::font_size, border_opacity),
         1},
        {new Button(
             "about", v, v, "About", []() { SDL_OpenURL("https://github.com/hxngxd"); }, 2.0f * Screen::font_size,
             border_opacity),
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
             2.0f * Screen::font_size, border_opacity),
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

    Map::current_map = Game::properties["map_init"].i;
    Map::LoadMap();

    Canvas *canvas0 =
        new Canvas("Play-0", v, Vector2(Screen::tile_size * 15, Screen::tile_size), 255, 0, 0, 0, border_opacity);

    canvas0->AddComponents({
        {new Text("score", v, v, "Score: 0", 1, Screen::font_size, border_opacity), 3},
        {new Text("time", v, v, Game::properties["show_time"].b ? "Time: 00:00:00.000" : "Time: ~_~", 1,
                  Screen::font_size, border_opacity),
         4},
        {new Text("map", v, v, "Map: " + str(Map::current_map), 1, Screen::font_size, border_opacity), 3},
        {new Text("dif", v, v, "Difficulty: idk", 1, Screen::font_size, border_opacity), 5},
    });

    Canvas *canvas1 = new Canvas("Play-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                                 Vector2(Screen::tile_size * 3, Screen::tile_size), 255, 0, 0, 0, border_opacity);

    canvas1->AddComponents({
        {new Text("hp", v, v, "Health: " + (Game::properties["show_health"].b ? str(100) : "~_~"), 1, Screen::font_size,
                  border_opacity),
         3},
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

    Canvas *canvas0 = new Canvas("MapBuilding-0", v, Vector2(Screen::tile_size * 10, Screen::tile_size), 255, 0, 0, 0,
                                 border_opacity);

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
             Screen::font_size, border_opacity),
         2},
        {new Button("save", v, v, "Save", MapMaking::Save, Screen::font_size, border_opacity), 2},
        {new Button("random", v, v, "Random", MapMaking::Random, Screen::font_size, border_opacity), 2},
        {new Button(
             "prev", v, v, "<",
             []() {
                 if (Map::current_map <= 1)
                     return;
                 Map::current_map--;
                 MapMaking::ChangeMap();
             },
             Screen::font_size, border_opacity),
         1},
        {new Text("curmap", v, v, "Map: " + str(Map::current_map), 1, Screen::font_size, border_opacity), 2},
        {new Button(
             "next", v, v, ">",
             []() {
                 Map::current_map++;
                 MapMaking::ChangeMap();
             },
             Screen::font_size, border_opacity),
         1},
    });

    Canvas *canvas1 = new Canvas("MapBuilding-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                                 Vector2(Screen::tile_size * 10, Screen::tile_size), 255, 0, 0, 0, border_opacity);

    auto change_drawing_type = [](int type) {
        MapMaking::current_drawing_type = (MapMaking::current_drawing_type == type ? -1 : type);
    };

    canvas1->AddComponents({
        {new Button("erase", v, v, "EMPTY", std::bind(change_drawing_type, EMPTY), Screen::font_size, border_opacity),
         2},
        {new Button("wall", v, v, "WALL", std::bind(change_drawing_type, WALL), Screen::font_size, border_opacity), 2},
        {new Button("coin", v, v, "COIN", std::bind(change_drawing_type, COIN), Screen::font_size, border_opacity), 2},
        {new Button("spawn", v, v, "SPAWN", std::bind(change_drawing_type, SPAWN), Screen::font_size, border_opacity),
         2},
        {new Button("win", v, v, "WIN", std::bind(change_drawing_type, WIN), Screen::font_size, border_opacity), 2},
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
                                 Vector2(Screen::tile_size, Screen::tile_size * 3), 255, 0, 0, 1, border_opacity);

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
             Screen::font_size, border_opacity),
         1},
        {new Button(
             "home", v, v, "Home",
             []() {
                 print("going back home...");
                 UI::SetVisible("Settings", false);
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
             Screen::font_size, border_opacity),
         1},
        {new Button(
             "settings", v, v, "Settings",
             []() {
                 MapMaking::allow_drawing = false;
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
             Screen::font_size, border_opacity),
         1},
    });
}

void Scene::SpawnPlayer()
{
    LinkedFunction *lf = new LinkedFunction(
        []() {
            print("creating player...");
            if (Game::properties["sound"].b)
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
    Canvas *canvas0 = new Canvas("Settings", Vector2(Screen::tile_size),
                                 Screen::resolution - Vector2(Screen::tile_size * 2), 240, 0, 0, 1, border_opacity);

    UI::SetVisible("Settings", false);

    canvas0->AddComponent(new Text("title", v, v, "SETTINGS", 1, 2.0f * Screen::font_size, border_opacity), 2);

    std::vector<Canvas *> canvases = {canvas0};

    for (int i = 1; i <= 15; i++)
    {
        canvases.push_back(new Canvas("Section-" + str(i), v, v, 0, 0, 0, 0, border_opacity));
        canvas0->AddComponent(canvases[i]);
    }

    canvases[1]->AddComponents({
        {new Text("resolution", v, v, "Resolution (restart required)", 0, Screen::font_size), 4},
        {new Button(
             "prev", v, v, "<",
             []() {
                 if (Game::properties["resolution"].i > 0)
                     Game::properties["resolution"].i--;
                 Text::SetLabel("Settings.Section-1.curres",
                                str(Screen::resolutions[Game::properties["resolution"].i]) + " x " +
                                    str(Screen::resolutions[Game::properties["resolution"].i]));
             },
             Screen::font_size),
         1},
        {new Text("curres", v, v,
                  str(Screen::resolutions[Game::properties["resolution"].i]) + " x " +
                      str(Screen::resolutions[Game::properties["resolution"].i]),
                  1, Screen::font_size),
         2},
        {new Button(
             "next", v, v, ">",
             []() {
                 if (Game::properties["resolution"].i < (int)(Screen::resolutions.size() - 1))
                     Game::properties["resolution"].i++;
                 Text::SetLabel("Settings.Section-1.curres",
                                str(Screen::resolutions[Game::properties["resolution"].i]) + " x " +
                                    str(Screen::resolutions[Game::properties["resolution"].i]));
             },
             Screen::font_size),
         1},
    });

    canvases[2]->AddComponents({
        {new Text("movespeed", v, v, "Move speed", 0, Screen::font_size), 2},
        {new Slider(
             "movespeedslider", v, v, 0.5f, 2.5f, Game::properties["player_move_speed"].f, 0.1f,
             [](float &value) {
                 Game::properties["player_move_speed"].f = value;
                 Screen::CalculateMoveSpeed();
             },
             Screen::font_size),
         3},
    });

    canvases[3]->AddComponents({{new Text("jumpspeed", v, v, "Jump speed", 0, Screen::font_size), 2},
                                {new Slider(
                                     "jumpspeedslider", v, v, 0.5f, 2.5f, Game::properties["player_jump_speed"].f, 0.1f,
                                     [](float &value) {
                                         Game::properties["player_jump_speed"].f = value;
                                         Screen::CalculateJumpSpeed();
                                     },
                                     Screen::font_size),
                                 3}});

    canvases[4]->AddComponents(
        {{new Text("acceleration", v, v, "Acceleration", 0, Screen::font_size), 2},
         {new Slider(
              "accelerationslider", v, v, 0.25f, 2.0f, Game::properties["player_acceleration"].f, 0.1f,
              [](float &value) {
                  Game::properties["player_acceleration"].f = value;
                  Screen::CalculateAcceleration();
              },
              Screen::font_size),
          3}});

    canvases[5]->AddComponents({
        {new Text("gravity", v, v, "Gravity", 0, Screen::font_size), 2},
        {new Slider(
             "gravityslider", v, v, 0.0f, 10.0f, Game::properties["gravity"].f, 0.1f,
             [](float &value) {
                 Game::properties["gravity"].f = value;
                 Screen::CalculateGravity();
             },
             Screen::font_size),
         3},
    });

    canvases[6]->AddComponents({
        {new Text("immortal", v, v, "Immortal", 1, Screen::font_size), 3},
        {new Toggle("immortaltoggle", v, v, Game::properties["player_immortal"].b,
                    [](bool &option) { Game::properties["player_immortal"].b = option; }),
         1},

        {new Text("keyboard", v, v, "Key layout", 1, Screen::font_size), 3},
        {new Toggle("keyboardtoggle", v, v, Game::properties["keyboard_layout"].b,
                    [](bool &option) { Game::properties["keyboard_layout"].b = option; }),
         1},
    });

    canvases[7]->AddComponents({
        {new Button("selectmusic", v, v, "Select music", SelectMusic, Screen::font_size), 4},
        {new Text("sound", v, v, "SoundFX", 1, Screen::font_size), 3},
        {new Toggle("soundtoggle", v, v, Game::properties["sound"].b,
                    [](bool &option) {
                        Game::properties["sound"].b = option;
                        if (!option)
                            StopAllSound();
                    }),
         1},
    });

    canvases[8]->AddComponents({
        {new Text("volume", v, v, "Volume", 0, Screen::font_size), 2},
        {new Slider(
             "volumeslider", v, v, 0.0f, 127.0f, Game::properties["volume"].i, 1.0f,
             [](float &value) {
                 Game::properties["volume"].i = (int)value;
                 Mix_VolumeMusic(value);
             },
             Screen::font_size),
         3},
    });

    canvases[9]->AddComponents({
        {new Text("background", v, v, "Background", 1, Screen::font_size), 3},
        {new Toggle("backgroundtoggle", v, v, Game::properties["background_enable"].b,
                    [](bool &option) { Game::properties["background_enable"].b = option; }),
         1},
        {new Text("grid", v, v, "Point grid", 1, Screen::font_size), 3},
        {new Toggle("gridtoggle", v, v, Game::properties["point_grid"].b,
                    [](bool &option) { Game::properties["point_grid"].b = option; }),
         1},
    });

    canvases[10]->AddComponents({
        {new Text("ray", v, v, "Ray opacity", 0, Screen::font_size), 2},
        {new Slider(
             "rayslider", v, v, 0.0f, 255.0f, Game::properties["ray_opacity"].i, 0.1f,
             [](float &value) { Game::properties["ray_opacity"].i = value; }, Screen::font_size),
         3},
    });

    canvases[11]->AddComponents({
        {new Text("mapdelay", v, v, "Map animation delay", 0, Screen::font_size), 2},
        {new Slider(
             "mapdelayslider", v, v, 0.0f, 50.0f, Game::properties["map_delay"].f, 0.1f,
             [](float &value) { Game::properties["map_delay"].f = value; }, Screen::font_size),
         3},
    });

    canvases[12]->AddComponents({
        {new Text("time", v, v, "Show time", 1, Screen::font_size), 2},
        {new Toggle("timetoggle", v, v, Game::properties["show_time"].b,
                    [](bool &option) {
                        Game::properties["show_time"].b = option;
                        Text::SetLabel("Play-0.time", "Time: ~_~");
                    }),
         1},
        {new Text("resethealth", v, v, "Reset health", 1, Screen::font_size), 2},
        {new Toggle("resethealthtoggle", v, v, Game::properties["reset_health"].b,
                    [](bool &option) { Game::properties["reset_health"].b = option; }),
         1},
        {new Text("showhealth", v, v, "Show health", 1, Screen::font_size), 2},
        {new Toggle("showhealthtoggle", v, v, Game::properties["show_health"].b,
                    [](bool &option) {
                        Game::properties["show_health"].b = option;
                        Text::SetLabel("Play-1.hp", "Health: ~_~");
                    }),
         1},
    });

    canvases[13]->AddComponents({
        {new Text("wallpossibility", v, v, "Map building: Wall (%)", 0, Screen::font_size), 2},
        {new Slider(
             "wallpossibilityslider", v, v, 0.0f, 35.0f, Game::properties["wall_possibility"].i, 1.0f,
             [](float &value) { Game::properties["wall_possibility"].i = (int)value; }, Screen::font_size),
         3},
    });

    canvases[14]->AddComponents({
        {new Text("coinpossibility", v, v, "Map building: Coin (%)", 0, Screen::font_size), 2},
        {new Slider(
             "coinpossibilityslider", v, v, 5.0f, 50.0f, Game::properties["coin_possibility"].i, 1.0f,
             [](float &value) { Game::properties["coin_possibility"].i = (int)value; }, Screen::font_size),
         3},
    });

    canvases[15]->AddComponents({
        {new Button("save", v, v, "Save", Game::SaveConfig, Screen::font_size, border_opacity), 2},
        {new Button(
             "default", v, v, "Default settings (also quit game)",
             []() {
                 Game::LoadConfig(1);
                 Game::SaveConfig();
                 Game::running = false;
             },
             Screen::font_size, border_opacity),
         3},
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
                     MapMaking::allow_drawing = true;
                     break;
                 }
             },
             Screen::font_size, border_opacity),
         1},
    });
}

void Scene::SelectMusic()
{
    UI::SetVisible("Settings", false);

    Canvas *canvas0 = new Canvas("SelectMusic", Vector2(Screen::tile_size),
                                 Screen::resolution - Vector2(Screen::tile_size * 2), 240, 0, 0, 1, border_opacity);

    canvas0->AddComponents({
        {new Text("title", v, v, "SELECT MUSIC", 1, 2.0f * Screen::font_size, border_opacity), 2},
        {new Text("curmusic", v, v, "Current selection: ", 1, Screen::font_size, border_opacity), 1},
    });

    std::vector<std::pair<std::string, std::string>> paths;
    for (auto &entry : std::filesystem::directory_iterator("sound/musics"))
    {
        std::string fullname = entry.path().string();
        if (fullname.size() >= 4 && fullname.substr(fullname.size() - 4) == ".mp3")
        {
            size_t lastSlash = fullname.find_last_of('\\');
            std::string name = fullname.substr(lastSlash + 1);

            size_t lastDot = name.find_last_of('.');
            if (lastDot != std::string::npos)
                name = name.substr(0, lastDot);

            if (name.size() > 20)
                name = name.substr(0, 20) + "....mp3";
            else
                name += ".mp3";

            paths.push_back({name, entry.path().string()});

            if (paths.size() > 30)
                break;
        }
    }

    Canvas *canvas1 = new Canvas("Section-1", v, v, 0, 0, 0, 0, border_opacity);

    canvas0->AddComponent(canvas1, (paths.size() + 1) / 2);

    Canvas *canvas10 = new Canvas("Section-10", v, v, 0, 0, 0, 1, border_opacity);

    int cnt = 1;
    for (int i = 0; i < (paths.size() + 1) / 2; i++)
    {
        canvas10->AddComponent(new Button(
            str(cnt) + paths[i].first, v, v, str(i) + ".  " + paths[i].first, []() {}, Screen::font_size,
            border_opacity));
        cnt++;
    }

    Canvas *canvas11 = new Canvas("Section-11", v, v, 0, 0, 0, 1, border_opacity);

    for (int i = (paths.size() + 1) / 2; i < paths.size(); i++)
    {
        canvas11->AddComponent(new Button(
            str(cnt) + paths[i].first, v, v, str(i) + ".  " + paths[i].first, []() {}, Screen::font_size,
            border_opacity));
    }

    canvas1->AddComponents({
        {canvas10, 1},
        {canvas11, 1},
    });

    Canvas *lastCanvas = new Canvas("last", v, v, 0, 0, 0, 0, border_opacity);
    canvas0->AddComponent(lastCanvas);

    lastCanvas->AddComponents({
        {new Button(
             "off", v, v, "Music off", []() {}, Screen::font_size, border_opacity),
         2},
        {new Button(
             "save", v, v, "Save", []() {}, Screen::font_size, border_opacity),
         2},
        {new Button(
             "exit", v, v, "Exit",
             []() {
                 UI::SetVisible("Settings", true);
                 UI::RemovingUI("SelectMusic");
             },
             Screen::font_size, border_opacity),
         1},
    });
}