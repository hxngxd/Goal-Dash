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

    Vector2 v;

    Canvas *canvas = new Canvas(
        "welcome", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 128, 8, 8, 1);
    canvas->AddComponents({
        {new Text("title", v, v, "GOAL DASH", 1, 55), 2},
        {new Button("start", v, v, "Start", Play, 50), 1},
        {new Button("mapbuilding", v, v, "Map Building", MapMaking, 50), 1},
    });

    Canvas *canvas1 = new Canvas("section1", v, v, 0, 8, 0, 0);
    canvas1->AddComponents({
        {new Button("settings", v, v, "Settings", Settings, 50), 1},
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
}

void Scene::Play()
{
    print("entering play mode...");
    Map::mode = 0;
    UI::RemovingUIs();
    Map::current_map == Game::properties["map_init"].i;
    Map::LoadMap();

    Canvas *canvas0 = new Canvas("play0", Vector2(), Vector2(Screen::tile_size * 15, Screen::tile_size), 255, 0, 0, 0);

    canvas0->AddComponents({
        {new Text("score", Vector2(), Vector2(), "Score: 0", 1, 25), 3},
        {new Text("time", Vector2(), Vector2(), "Time: 00:00:00.000", 1, 25), 4},
        {new Text("map", Vector2(), Vector2(), "Map: " + str(Map::current_map), 1, 25), 3},
        {new Text("dif", Vector2(), Vector2(), "Difficulty: idk", 1, 25), 5},
    });

    Canvas *canvas1 = new Canvas("play1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                                 Vector2(Screen::tile_size * 3, Screen::tile_size), 255, 0, 0, 0);

    canvas1->AddComponents({
        {new Text("hp", Vector2(), Vector2(), "Health: 100", 1, 25), 3},
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

    Canvas *canvas0 =
        new Canvas("mapbulding0", Vector2(), Vector2(Screen::tile_size * 10, Screen::tile_size), 255, 0, 0, 0);

    canvas0->AddComponents({
        {new Button(
             "clear", Vector2(), Vector2(), "Clear",
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
        {new Button("save", Vector2(), Vector2(), "Save", MapMaking::Save, 25), 2},
        {new Button("random", Vector2(), Vector2(), "Random", MapMaking::Random, 25), 2},
        {new Button(
             "prev", Vector2(), Vector2(), "<",
             []() {
                 if (Map::current_map <= 1)
                     return;
                 Map::current_map--;
                 MapMaking::ChangeMap();
             },
             25),
         1},
        {new Text("curmap", Vector2(), Vector2(), "Map: " + str(Map::current_map), 1, 25), 2},
        {new Button(
             "next", Vector2(), Vector2(), ">",
             []() {
                 Map::current_map++;
                 MapMaking::ChangeMap();
             },
             25),
         1},
    });

    Canvas *canvas1 = new Canvas("mapbulding1", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
                                 Vector2(Screen::tile_size * 10, Screen::tile_size), 255, 0, 0, 0);

    auto change_drawing_type = [](int type) {
        MapMaking::current_drawing_type = (MapMaking::current_drawing_type == type ? -1 : type);
    };

    canvas1->AddComponents({
        {new Button("erase", Vector2(), Vector2(), "EMPTY", std::bind(change_drawing_type, EMPTY), 25), 2},
        {new Button("wall", Vector2(), Vector2(), "WALL", std::bind(change_drawing_type, WALL), 25), 2},
        {new Button("coin", Vector2(), Vector2(), "COIN", std::bind(change_drawing_type, COIN), 25), 2},
        {new Button("spawn", Vector2(), Vector2(), "SPAWN", std::bind(change_drawing_type, SPAWN), 25), 2},
        {new Button("win", Vector2(), Vector2(), "WIN", std::bind(change_drawing_type, WIN), 25), 2},
    });

    Common();

    Map::AddTiles();

    print("done");
}

void Scene::Common()
{

    Canvas *canvas0 = new Canvas("common", Vector2(Screen::tile_size * (Screen::map_size - 1), 0),
                                 Vector2(Screen::tile_size, Screen::tile_size * 4), 255, 0, 0, 1);

    canvas0->AddComponents({
        {new Button(
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
             25),
         1},
        {new Button(
             "home", Vector2(), Vector2(), "Home",
             []() {
                 print("going back home...");
                 auto goHome = []() {
                     LinkedFunction *lf = new LinkedFunction(
                         []() {
                             if (Map::mode)
                             {
                                 MapMaking::allow_drawing = false;
                                 MapMaking::current_drawing_type = -1;
                             }
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
             "settings", Vector2(), Vector2(), "Settings", []() {}, 25),
         1},
        {new Button(
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
    // UI::RemovingUIs();

    // Vector2 v;

    // std::string canvas = "settings-canvas";
    // std::string title = canvas + "-title";

    // std::string move = "-move-speed";
    // std::string move_slider = move + "-slider";

    // std::string jump = "-jump-speed";
    // std::string jump_slider = jump + "-slider";

    // std::string acceleration = "-acceleration";
    // std::string acceleration_slider = acceleration + "-slider";

    // std::string gravity = "-gravity";
    // std::string gravity_slider = gravity + "-slider";

    // std::string immortal = "-immortal";
    // std::string immortal_toggle = immortal + "-toggle";

    // std::string collision = "-collision";
    // std::string collision_toggle = collision + "-toggle";

    // std::string key = "-keylayout";
    // std::string key_toggle = key + "-toggle";

    // std::string music = "-music";
    // std::string music_toggle = music + "-toggle";

    // std::string sound = "-sound";
    // std::string sound_toggle = sound + "-toggle";

    // std::string volume = "-volume";
    // std::string volume_slider = volume + "-slider";

    // std::string background = "-background";
    // std::string background_toggle = background + "-toggle";

    // std::string grid = "-grid";
    // std::string grid_toggle = grid + "-toggle";

    // std::string ray = "-ray";
    // std::string ray_slider = ray + "-slider";

    // std::string mapinit = "-mapinit";
    // std::string prevmap = mapinit + "-prevmap";
    // std::string curmap = mapinit + "-curmap";
    // std::string nextmap = mapinit + "-nextmap";

    // std::string mapdelay = "-mapdelay";
    // std::string mapdelay_slider = mapdelay + "-slider";

    // std::string save = "-save";
    // std::string default_ = "-default";
    // std::string exit = "-exit";

    // std::vector<Canvas *> canvases;

    // for (int i = 0; i < 11; i++)
    // {
    //     canvases.push_back(new Canvas(canvas + "-" + str(i), v, v, 0, 0, 0, 0));
    // }

    // canvases.

    // std::vector<UI *> settings_uis = {
    //     new Canvas(canvas, Vector2(Screen::tile_size), Screen::resolution - Vector2(Screen::tile_size * 2), 180, 0,
    //     8,
    //                1),
    //     new Text(title, v, v, "SETTINGS", 1, 50),

    //     new Text(move, v, v, "Move speed", 1, 25),
    //     new Slider(move_slider, v, v, 0.5f, 2.5f, 1.0f, 0.1f, 20),

    //     new Text(jump, v, v, "Jump speed", 1, 25),
    //     new Slider(jump_slider, v, v, 0.5f, 2.5f, 1.0f, 0.1f, 20),

    //     new Text(acceleration, v, v, "Acceleration", 1, 25),
    //     new Slider(acceleration_slider, v, v, 0.25f, 2.0f, 1.0f, 0.1f, 20),

    //     new Text(gravity, v, v, "Gravity", 1, 25),
    //     new Slider(gravity_slider, v, v, 0.0f, 10.0f, 5.0f, 0.1f, 20),

    //     new Canvas(canvas0, v, v, 0, 0, 0, 0),

    //     new Text(immortal, v, v, "Immortal", 1, 25),
    //     new Toggle(immortal_toggle, v, v, false),

    //     new Text(collision, v, v, "Map collision", 1, 25),
    //     new Toggle(collision_toggle, v, v, true),

    //     new Text(key, v, v, "Key layout", 1, 25),
    //     new Toggle(key_toggle, v, v, false),

    //     new Canvas(canvas1, v, v, 0, 0, 0, 0),

    //     new Text(music, v, v, "Music", 1, 25),
    //     new Toggle(music_toggle, v, v, false),

    //     new Text(sound, v, v, "SoundFX", 1, 25),
    //     new Toggle(sound_toggle, v, v, false),

    //     new Text(volume, v, v, "Volume", 1, 25),
    //     new Slider(volume_slider, v, v, 0.0f, 127.0f, 64.0f, 1.0f, 20),

    //     new Canvas(canvas2, v, v, 0, 0, 0, 0),

    //     new Text(background, v, v, "Background", 0, 25),
    //     new Toggle(background_toggle, v, v, true),

    //     new Text(grid, v, v, "Point grid", 1, 25),
    //     new Toggle(grid_toggle, v, v, true),

    //     new Text(ray, v, v, "Ray opacity", 1, 25),
    //     new Slider(ray_slider, v, v, 0.0f, 255.0f, 180.0f, 0.1f, 20),

    //     new Canvas(lastcanvas, v, v, 0, 0, 0, 0),

    //     new Button(
    //         save, v, v, "Save", []() {}, 25),
    //     new Button(
    //         default_, v, v, "Default Settings", []() {}, 25),
    //     new Button(
    //         exit, v, v, "Exit", []() {}, 25),
    // };
}