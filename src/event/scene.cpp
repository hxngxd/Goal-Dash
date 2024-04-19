#include "scene.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../event/input.h"
#include "../game.h"
#include "../gameobject/gameobject.h"
#include "../gameobject/map.h"
#include "ui.h"
#include <filesystem>

Vector2 v;

int border_opacity = 32;

void Scene::Welcome()
{
    Map::mode = -1;
    print("creating welcome scene...");

    Canvas *canvas = new Canvas(
        "Welcome", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 240, 8, 8, 1, border_opacity);
    canvas->AddComponents({
        {new Text("title", v, v, "GOAL DASH", 1, 2.5f * Screen::font_size, border_opacity), 2},
        {new Button("start", v, v, "Start", SelectMap, 2.0f * Screen::font_size, border_opacity), 1},
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
    Player::total_score = 0;
    Player::hp = 100;

    UI::RemovingUI("Welcome");
    UI::RemovingUI("SelectMap");

    MapMaking::ToggleBtns(false);

    Map::LoadMap(Map::MapPlaylist[Map::current_map_id].second);

    Canvas *canvas0 =
        new Canvas("Play-0", v, Vector2(Screen::tile_size * 15, Screen::tile_size), 255, 0, 0, 0, border_opacity);

    canvas0->AddComponents({
        {new Text("score", v, v, "Score: 0", 1, Screen::font_size, border_opacity), 3},
        {new Text("time", v, v, Game::properties["show_time"].b ? "Time: 00:00:00.000" : "Time: ~_~", 1,
                  Screen::font_size, border_opacity),
         4},
        {new Text("map", v, v, "Map: " + getFileName(Map::MapPlaylist[Map::current_map_id].second, 12), 1,
                  Screen::font_size, border_opacity),
         3},
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
    UI::RemovingUI("Welcome");
    Map::count_types.clear();
    Map::MapPlaylist.clear();

    Canvas *canvas0 = new Canvas("MapBuilding-0", v, Vector2(Screen::tile_size * 15, Screen::tile_size), 255, 0, 0, 0,
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
        {new Button("save", v, v, "Save", SaveChoice, Screen::font_size, border_opacity), 2},
        {new Button("random", v, v, "Random", MapMaking::Random, Screen::font_size, border_opacity), 2},
        {new Button(
             "delete", v, v, "Delete",
             []() {
                 bool success = MapMaking::Delete();
                 if (success)
                     ShowMessage("Map deleted.");
                 else
                     ShowMessage("Failed to delete map.");
             },
             Screen::font_size, border_opacity),
         2},
        {new Button("curmap", v, v, "No map selected", SelectMap, Screen::font_size, border_opacity), 6},
    });

    Canvas *canvas1 =
        new Canvas("MapBuilding-1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                   Vector2(Screen::tile_size * Screen::map_size, Screen::tile_size), 255, 0, 0, 0, border_opacity);

    auto change_drawing_type = [](int type) {
        MapMaking::current_drawing_type = (MapMaking::current_drawing_type == type ? -1 : type);
    };

    canvas1->AddComponents({
        {new Button("erase", v, v, "EMPTY", std::bind(change_drawing_type, EMPTY), Screen::font_size, border_opacity),
         1},
        {new Button("wall", v, v, "WALL", std::bind(change_drawing_type, WALL), Screen::font_size, border_opacity), 1},
        {new Button("coin", v, v, "COIN", std::bind(change_drawing_type, COIN), Screen::font_size, border_opacity), 1},
        {new Button("health", v, v, "HEALTH", std::bind(change_drawing_type, HEALTH), Screen::font_size,
                    border_opacity),
         1},
        {new Button("spawn", v, v, "SPAWN", std::bind(change_drawing_type, SPAWN), Screen::font_size, border_opacity),
         1},
        {new Button("win", v, v, "WIN", std::bind(change_drawing_type, WIN), Screen::font_size, border_opacity), 1},
    });

    UI::SetVisible("Common", true);

    MapMaking::ToggleBtns(true);

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
                                 Map::MapPlaylist.clear();
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
                PlaySound("spawn", CHANNEL_SPAWN_WIN_DIE, 0);
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

    canvas0->AddComponent(new Text("title", v, v, "SETTINGS", 1, 2.0f * Screen::font_size, border_opacity), 1);

    std::vector<Canvas *> canvases = {canvas0};

    for (int i = 1; i <= 16; i++)
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
             "wallpossibilityslider", v, v, 0.0f, 30.0f, Game::properties["wall_possibility"].i, 1.0f,
             [](float &value) { Game::properties["wall_possibility"].i = (int)value; }, Screen::font_size),
         3},
    });

    canvases[14]->AddComponents({
        {new Text("coinpossibility", v, v, "Map building: Coin (%)", 0, Screen::font_size), 2},
        {new Slider(
             "coinpossibilityslider", v, v, 1.0f, 30.0f, Game::properties["coin_possibility"].i, 1.0f,
             [](float &value) { Game::properties["coin_possibility"].i = (int)value; }, Screen::font_size),
         3},
    });

    canvases[15]->AddComponents({
        {new Text("coinpossibility", v, v, "Map building: Health (%)", 0, Screen::font_size), 2},
        {new Slider(
             "coinpossibilityslider", v, v, 0.0f, 30.0f, Game::properties["health_possibility"].i, 1.0f,
             [](float &value) { Game::properties["health_possibility"].i = (int)value; }, Screen::font_size),
         3},
    });

    canvases[16]->AddComponents({
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
    UI::SetVisible("Common", false);

    Canvas *canvas0 = new Canvas("SelectMusic", Vector2(Screen::tile_size),
                                 Screen::resolution - Vector2(Screen::tile_size * 2), 240, 0, 0, 1, border_opacity);

    canvas0->AddComponents({
        {new Text("title", v, v, "SELECT MUSIC", 1, 2.0f * Screen::font_size, border_opacity), 1},
        {new Text("curmusic", v, v,
                  "Playing: " +
                      (Game::properties["music"].s == "off" ? "Off" : getFileName(Game::properties["music"].s, 25)),
                  1, Screen::font_size, border_opacity),
         1},
    });

    Canvas *canvas1 = new Canvas("Section-1", v, v, 0, 0, 0, 0, border_opacity);

    canvas0->AddComponent(canvas1, 10);

    Canvas *canvas10 = new Canvas("Section-10", v, v, 0, 0, 0, 1, border_opacity);

    Canvas *canvas11 = new Canvas("Section-11", v, v, 0, 0, 0, 1, border_opacity);

    canvas1->AddComponents({
        {canvas10, 1},
        {canvas11, 1},
    });

    int numOfMusic = 0;
    for (auto &entry : std::filesystem::directory_iterator("sound\\musics"))
    {
        std::string path = entry.path().string();
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".mp3")
            numOfMusic++;
    }

    int cnt = 0;

    for (auto &entry : std::filesystem::directory_iterator("sound\\musics"))
    {
        std::string path = entry.path().string();
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".mp3")
        {
            Button *ms = new Button(str(cnt) + "-" + getFileName(path, 5), v, v, getFileName(path, 30),
                                    std::bind(
                                        [](std::string path) {
                                            Game::properties["music"].s = path;
                                            if (Music)
                                            {
                                                if (Mix_PlayingMusic())
                                                    StopMusic();
                                                Mix_FreeMusic(Music);
                                                Music = nullptr;
                                            }
                                            LoadMusic(path);
                                            PlayMusic(-1);
                                            Mix_VolumeMusic(Game::properties["volume"].i);
                                            Text::SetLabel("SelectMusic.curmusic", "Playing: " + getFileName(path, 30));
                                        },
                                        path),
                                    Screen::font_size, border_opacity);

            if (cnt < (numOfMusic + 1) / 2)
                canvas10->AddComponent(ms);
            else
                canvas11->AddComponent(ms);

            cnt++;
        }
    }

    Canvas *canvas2 = new Canvas("Section-2", v, v, 0, 0, 0, 0, border_opacity);

    canvas0->AddComponent(canvas2);

    canvas2->AddComponents({
        {new Text("volume", v, v, "Volume", 0, Screen::font_size), 2},
        {new Slider(
             "volumeslider", v, v, 0.0f, 127.0f, Game::properties["volume"].i, 1.0f,
             [](float &value) {
                 Game::properties["volume"].i = (int)value;
                 ((Slider *)UI::UIs["Settings.Section-8.volumeslider"])->current_value = value;
                 Mix_VolumeMusic(value);
             },
             Screen::font_size),
         3},
    });

    Canvas *lastCanvas = new Canvas("last", v, v, 0, 0, 0, 0, border_opacity);

    canvas0->AddComponent(lastCanvas);

    lastCanvas->AddComponents({
        {new Button(
             "off", v, v, "Music off",
             []() {
                 Game::properties["music"].s = "off";
                 Text::SetLabel("SelectMusic.curmusic", "Playing: Off");
                 if (!Music)
                     return;
                 StopMusic();
                 Mix_FreeMusic(Music);
                 Music = nullptr;
             },
             Screen::font_size, border_opacity),
         2},
        {new Button("save", v, v, "Save", Game::SaveConfig, Screen::font_size, border_opacity), 2},
        {new Button(
             "goback", v, v, "Back",
             []() {
                 UI::SetVisible("Settings", true);
                 UI::SetVisible("Common", true);
                 UI::RemovingUI("SelectMusic");
             },
             Screen::font_size, border_opacity),
         1},
    });
}

bool current_toggle = false;

void selectMap(std::pair<std::string, std::string> map)
{
    print(map.second, "selected");
    Map::MapPlaylist.push_back(map);
    ((Button *)UI::UIs["SelectMap.last.ok"])->enabled = true;
}

int deselectMap(std::pair<std::string, std::string> map)
{
    print(map.second, "deselected");
    auto id = std::find(Map::MapPlaylist.begin(), Map::MapPlaylist.end(), map);
    Map::MapPlaylist.erase(id);
    if (Map::MapPlaylist.size() == 0)
        ((Button *)UI::UIs["SelectMap.last.ok"])->enabled = false;
    return id - Map::MapPlaylist.begin();
}

std::vector<std::pair<std::string, std::string>> tmpMaps;

void Scene::SelectMap()
{
    Map::MapPlaylist.clear();
    tmpMaps.clear();
    Map::current_map_id = 0;

    if (Map::mode == -1)
    {
        UI::SetVisible("Welcome", false);
    }
    else if (Map::mode == 1)
    {
        UI::SetVisible("MapBuilding-0", false);
        UI::SetVisible("MapBuilding-1", false);
        UI::SetVisible("Common", false);
    }

    Canvas *canvas0 = new Canvas("SelectMap", Vector2(Screen::tile_size),
                                 Screen::resolution - Vector2(Screen::tile_size * 2), 240, 0, 0, 1, border_opacity);

    canvas0->AddComponent(new Text("title", v, v, "SELECT MAP", 1, 2.0f * Screen::font_size, border_opacity));

    Canvas *canvas1 = new Canvas("Section-1", v, v, 0, 0, 0, 0, border_opacity);

    canvas0->AddComponent(canvas1, Map::mode == 1 ? 13 : 12);

    Canvas *canvas10 = new Canvas("Section-10", v, v, 0, 0, 0, 1, border_opacity);

    Canvas *canvas11 = new Canvas("Section-11", v, v, 0, 0, 0, 1, border_opacity);

    canvas1->AddComponents({
        {canvas10, 1},
        {canvas11, 1},
    });

    int numOfMap = 0;

    for (auto &entry : std::filesystem::directory_iterator("map"))
    {
        std::string path = entry.path().string();
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".map")
            numOfMap++;
    }

    int cnt = 0;

    for (auto &entry : std::filesystem::directory_iterator("map"))
    {
        std::string path = entry.path().string();
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".map")
        {
            Button *mp = new Button(
                str(cnt) + "-" + path, v, v, getFileName(path, 12), []() {}, Screen::font_size, border_opacity);

            if (cnt < (numOfMap + 1) / 2)
                canvas10->AddComponent(mp);
            else
                canvas11->AddComponent(mp);

            if (Map::mode == -1)
            {
                mp->onClick = std::bind(
                    [](std::string path, Button *mp) {
                        mp->selected = !mp->selected;
                        if (mp->selected)
                        {
                            selectMap(std::make_pair(mp->name, path));
                            mp->label += " (" + str(Map::MapPlaylist.size()) + ")";
                        }
                        else
                        {
                            int id = deselectMap(std::make_pair(mp->name, path));
                            mp->label = getFileName(path, 12);
                            for (int i = id; i < Map::MapPlaylist.size(); i++)
                            {
                                UI::UIs[Map::MapPlaylist[i].first]->label =
                                    getFileName(Map::MapPlaylist[i].second, 12) + " (" + str(i + 1) + ")";
                                ((Button *)UI::UIs[Map::MapPlaylist[i].first])->Recalculate();
                            }
                        }
                        mp->Recalculate();
                    },
                    path, mp);
            }
            else
            {
                mp->onClick = std::bind(
                    [](std::string path) {
                        MapMaking::ChangeMap(path);
                        Map::MapPlaylist.clear();
                        Map::MapPlaylist.push_back(std::make_pair("", path));
                        Text::SetLabel("MapBuilding-0.curmap", "Current map: " + getFileName(path, 12));
                        UI::RemovingUI("SelectMap");
                        UI::SetVisible("MapBuilding-0", true);
                        UI::SetVisible("MapBuilding-1", true);
                        UI::SetVisible("Common", true);
                    },
                    path);
            }

            tmpMaps.push_back(std::make_pair(mp->name, path));

            cnt++;
        }
    }

    if (Map::mode == -1)
    {
        Canvas *lastCanvas = new Canvas("last", v, v, 0, 0, 0, 0, border_opacity);
        canvas0->AddComponent(lastCanvas);

        lastCanvas->AddComponents({
            {new Button(
                 "toggleall", v, v, "All",
                 []() {
                     current_toggle = !current_toggle;
                     for (int i = 0; i < tmpMaps.size(); i++)
                     {
                         Button *btn = (Button *)UI::UIs[tmpMaps[i].first];
                         if (current_toggle)
                         {
                             if (!btn->selected)
                             {
                                 btn->selected = current_toggle;
                                 selectMap(tmpMaps[i]);
                                 btn->label += " (" + str(Map::MapPlaylist.size()) + ")";
                             }
                         }
                         else
                         {
                             if (btn->selected)
                             {
                                 btn->selected = current_toggle;
                                 deselectMap(tmpMaps[i]);
                                 btn->label = getFileName(tmpMaps[i].second, 12);
                             }
                         }
                         btn->Recalculate();
                     }
                 },
                 Screen::font_size, border_opacity),
             1},
            {new Button("ok", v, v, "Play now", Play, Screen::font_size, border_opacity), 1},
            {new Button(
                 "goback", v, v, "Back",
                 []() {
                     UI::SetVisible("Welcome", true);
                     UI::RemovingUI("SelectMap");
                 },
                 Screen::font_size, border_opacity),
             1},
        });
        ((Button *)UI::UIs["SelectMap.last.ok"])->enabled = false;
    }
    else
    {
        canvas0->AddComponent(new Button(
            "goback", v, v, "Back",
            []() {
                UI::RemovingUI("SelectMap");
                UI::SetVisible("MapBuilding-0", true);
                UI::SetVisible("MapBuilding-1", true);
                UI::SetVisible("Common", true);
            },
            Screen::font_size, border_opacity));
    }
}

void Scene::SaveChoice()
{
    UI::SetVisible("MapBuilding-0", false);
    UI::SetVisible("MapBuilding-1", false);
    UI::SetVisible("Common", false);

    MapMaking::allow_drawing = false;

    Canvas *canvas = new Canvas(
        "SaveChoice", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 6.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 3.0f), 240, 8, 8, 1, border_opacity);

    if (Map::MapPlaylist.size())
    {
        canvas->AddComponent(new Button(
            "tothismap", v, v, "Save current map",
            []() {
                bool success = MapMaking::Save(0);
                if (success)
                {
                    UI::SetVisible("MapBuilding-0", true);
                    UI::SetVisible("MapBuilding-1", true);
                    UI::SetVisible("Common", true);
                    UI::RemovingUI("SaveChoice");
                    MapMaking::allow_drawing = true;
                    ShowMessage("Map saved.");
                }
                else
                {
                    ShowMessage("Failed to save map.");
                }
            },
            Screen::font_size, border_opacity));
    }

    canvas->AddComponents({
        {new Button("tonewmap", v, v, "Save as new map", SaveAs, Screen::font_size, border_opacity), 1},
        {new Button(
             "back", v, v, "Back",
             []() {
                 UI::SetVisible("MapBuilding-0", true);
                 UI::SetVisible("MapBuilding-1", true);
                 UI::SetVisible("Common", true);
                 UI::RemovingUI("SaveChoice");
                 MapMaking::allow_drawing = true;
             },
             Screen::font_size, border_opacity),
         1},
    });
}

void Scene::SaveAs()
{
    UI::SetVisible("SaveChoice", false);

    Canvas *canvas0 = new Canvas(
        "SaveAs", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 3.0f, Screen::resolution.y / 10.0f),
        Vector2(Screen::resolution.x / 1.5f, Screen::resolution.y / 5.0f), 240, 8, 8, 1, border_opacity);

    Canvas *canvas1 = new Canvas("NameInput", v, v, 0, 8, 0, 0, border_opacity);

    Canvas *canvas2 = new Canvas("SaveOrBack", v, v, 0, 8, 0, 0);

    canvas0->AddComponents({
        {canvas1, 2},
        {canvas2, 1},
    });

    canvas1->AddComponents({
        {new Text("mapname", v, v, "Map name: ", 1, Screen::font_size), 1},
        {new Text("curmapname", v, v, "", 1, Screen::font_size, 0, 1), 3},
    });

    EventHandler::currentInputtingText = &UI::UIs["SaveAs.NameInput.curmapname"]->label;
    EventHandler::currentMaximumInputLength = 12;

    canvas2->AddComponents({
        {new Button(
             "ok", v, v, "Okay",
             []() {
                 std::string &label = UI::UIs["SaveAs.NameInput.curmapname"]->label;
                 if (label.size())
                 {
                     bool success = MapMaking::Save(1, label);
                     if (success)
                     {
                         UI::SetVisible("MapBuilding-0", true);
                         UI::SetVisible("MapBuilding-1", true);
                         UI::SetVisible("Common", true);
                         UI::RemovingUI("SaveAs");
                         UI::RemovingUI("SaveChoice");
                         EventHandler::currentInputtingText = nullptr;
                         Text::SetLabel("MapBuilding-0.curmap", "Current map: " + label);
                         MapMaking::allow_drawing = true;
                         ShowMessage("Map saved.");
                     }
                     else
                     {
                         ShowMessage("Failed to save map.");
                     }
                 }
                 else
                 {
                     ShowMessage("Failed to save map.");
                 }
             },
             Screen::font_size, border_opacity),
         1},
        {new Button(
             "back", v, v, "Go back",
             []() {
                 UI::SetVisible("SaveChoice", true);
                 UI::RemovingUI("SaveAs");
                 EventHandler::currentInputtingText = nullptr;
             },
             Screen::font_size, border_opacity),
         1},
    });
}

void Scene::ShowMessage(std::string message)
{
    EventHandler::allow_ui = false;
    Canvas *canvas0 =
        new Canvas("msgbg", Vector2(), Screen::resolution, 200, 0, Screen::resolution.x / 4.0f, 1, border_opacity);

    canvas0->AddComponent(new Text("msg", v, v, message, 1, Screen::font_size));

    LinkedFunction *lf = new LinkedFunction(
        []() {
            UI::RemovingUI("msgbg");
            EventHandler::allow_ui = true;
            return 1;
        },
        1000);
    lf->Execute();
}

void Scene::ShowWinOrLose(bool win, int time)
{
    if (Game::properties["sound"].b)
        PlaySound(win ? "win_scene" : "lose_scene", CHANNEL_SPAWN_WIN_DIE, 0);

    UI::RemovingUI("Play-0");
    UI::RemovingUI("Play-1");
    UI::SetVisible("Common", false);

    Canvas *canvas0 = new Canvas("winorlosebg", Vector2(), Screen::resolution, 180, 0, Screen::resolution.x / 5.0f, 1);

    Canvas *canvas1 = new Canvas("row1", v, v, 0, 0, 0);
    Canvas *canvas2 = new Canvas("row2", v, v, 240, 0, 0, 1, border_opacity);
    Canvas *canvas3 = new Canvas("row3", v, v, 0, 0, 0);
    canvas0->AddComponents({
        {canvas1, 1},
        {canvas2, 4},
        {canvas3, 1},
    });

    canvas2->AddComponents({
        {new Text("state", v, v, win ? "You passed!" : "You failed, try again next time!", 1, 1.5 * Screen::font_size),
         2},
        {new Text("score", v, v, "Scored: " + str(Player::total_score), 1, Screen::font_size), 1},
        {new Text("time", v, v, "In: " + FormatMS(time) + "s", 1, Screen::font_size), 1},
        {new Button(
             "again", v, v, "Play again",
             []() {
                 Map::current_map_id = 0;
                 UI::RemovingUI("winorlosebg");
                 Scene::Play();
             },
             Screen::font_size, border_opacity),
         1},
        {new Button(
             "home", v, v, "Go home",
             []() {
                 UI::RemovingUI("winorlosebg");
                 Scene::Welcome();
             },
             Screen::font_size, border_opacity),
         1},
    });
}