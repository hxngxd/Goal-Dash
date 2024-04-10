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
    print("creating border...");
    Map::Border();
    print("border created...");

    print("creating welcoming canvas...");
    Canvas *cv = new Canvas(
        "welcome", Screen::resolution / 2.0f - Vector2(Screen::resolution.x / 4.0f, Screen::resolution.y / 4.0f),
        Vector2(Screen::resolution.x / 2.0f, Screen::resolution.y / 2.0f), 0, 8, 8, 1);

    print("creating title...");
    Text *title = new Text("title", Vector2(), Vector2(), "GOAL DASH", 55);

    print("creating buttons...");
    Button *start = new Button(
        "start", Vector2(), Vector2(), "Start", []() {}, 50);
    Button *map = new Button(
        "map", Vector2(), Vector2(), "Map Building", []() {}, 50);

    Canvas *cv1 = new Canvas("cv1", Vector2(), Vector2(), 0, 8, 0, 0);

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

    cv->AddComponents({{"title", 2}, {"start", 1}, {"map", 1}, {"cv1", 1}, {"exit", 1}});

    cv1->AddComponents({{"settings", 1}, {"about", 1}});

    // Button *mapbutton = new Button(
    //     "map", "Map Making", Vector2(),
    //     []() {
    //         // LinkedFunction *lf = new LinkedFunction(
    //         //     []() {
    //         //         UI::DeleteUIs();
    //         //         MapHUD();
    //         //         MapTile::isMakingMap = true;
    //         //         MapTile::currentMap = 1;
    //         //         Game::scene = new Scene(false);
    //         //         MapTile::currentMap--;
    //         //         return 1;
    //         //     },
    //         //     250);
    //         // lf->Execute();
    //     },
    //     50, 0);

    print("done");
}

void Scene::Play()
{
}

void Scene::MapMaking()
{
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

// void Scene::DeleteScene()
// {
//     print("deleting current scene...");

//     if (Game::player)
//     {
//         print("deleting player...");
//         if (Game::Properties["sound"].b)
//             PlaySound("win", CHANNEL_SPAWN_WIN, 0);
//         LinkedFunction *lf = new LinkedFunction(
//             []() { return TransformValue(&Game::player->scale, 0.0f, Game::Properties["tile_rescale_speed"].f); });
//         lf->NextFunction([]() {
//             delete Game::player;
//             Game::player = nullptr;
//             Game::player_time[2] = SDL_GetTicks();
//             print("player deleted");
//             print("deleting tiles (without border)...");
//             MapTile::DeleteTiles();
//             return 1;
//         });
//         lf->NextFunction(
//             []() {
//                 if (Game::scene)
//                 {
//                     delete Game::scene;
//                     Game::scene = nullptr;
//                 }
//                 print("tiles deleted");
//                 print("scene deleted");
//                 return 1;
//             },
//             (MapTile::nEmptyTiles + 4) * Game::Properties["map_delay"].f);
//         lf->NextFunction([]() {
//             if (nextMap)
//                 Game::scene = new Scene(true);
//             else
//                 Game::scene = new Scene();
//             return 1;
//         });
//         lf->Execute();
//     }
//     else
//     {
//         delete this;
//     }
// }