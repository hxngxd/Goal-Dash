#include "scene.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../game.h"
#include "../gameobject/gameobject.h"
#include "ui.h"

bool nextMap = true;

Scene::Scene()
{
    print("creating new scene...");
    print("creating border...");
    MapTile::CreateBorder();
    print("border created...");

    print("creating welcoming canvas...");
    Canvas *cv =
        new Canvas("welcome", Screen::resolution / 2 - Vector2(Screen::resolution.x / 4, Screen::resolution.y / 5),
                   Vector2(Screen::resolution.x / 2, Screen::resolution.y / 2.5), 128, 8, 8, 1, 1);
    print("done");

    print("creating buttons...");
    Button *startbtn = new Button(
        "start", "Start", Vector2(),
        []() {
            Game::scene->DeleteScene();
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    UI::DeleteUIs();
                    PlayerHUD();
                    Game::scene = new Scene(true);
                    return 1;
                },
                250);
            lf->Execute();
        },
        50, 0);
    Button *settingsbtn = new Button(
        "settings", "Settings", Vector2(), []() {}, 50, 0);
    Button *aboutbtn = new Button(
        "about", "About", Vector2(), []() { SDL_OpenURL("https://github.com/hxngxd"); }, 50, 0);
    Button *mapbutton = new Button(
        "map", "Map Making", Vector2(),
        []() {
            Game::scene->DeleteScene();
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    UI::DeleteUIs();
                    MapHUD();
                    MapTile::isMakingMap = true;
                    MapTile::currentMap = 1;
                    Game::scene = new Scene(false);
                    MapTile::currentMap--;
                    return 1;
                },
                250);
            lf->Execute();
        },
        50, 0);
    Button *exitbtn = new Button(
        "exit", "Exit", Vector2(),
        []() {
            LinkedFunction *lf = new LinkedFunction(
                []() {
                    game->Stop();
                    return 1;
                },
                250);
            lf->Execute();
        },
        50, 0);
    Text *title = new Text("title", "Goal Dash", Vector2(), 150);
    cv->AddComponents({"title", "start", "settings", "map", "about", "exit"});

    print("buttons created");

    print("scene created");
}

Scene::Scene(bool create_player)
{
    print("creating new scene...");
    print("creating tiles...");
    MapTile::CreateTiles(!create_player);

    if (!create_player)
    {
        return;
    }
    LinkedFunction *lf = new LinkedFunction(
        std::bind([]() {
            print("tiles created");
            print("creating player...");
            PlaySound("spawn", CHANNEL_SPAWN_WIN, 0);

            Vector2 player_position(MapTile::SpawnTile.y * Screen::tile_size, MapTile::SpawnTile.x * Screen::tile_size);
            Game::player = new Player(player_position);
            if (!Game::player_time[0])
                Game::player_time[0] = SDL_GetTicks();
            Game::player_time[1] = SDL_GetTicks();
            if (Game::player_time[2])
                Game::player_time[0] += (Game::player_time[1] - Game::player_time[2]);
            return 1;
        }),
        (MapTile::nEmptyTiles + 4) * Game::Properties["map_animation_delay"].f);
    lf->NextFunction([]() { return TransformValue(&Game::player->scale, 1.0f, Game::Properties["rescale_speed"].f); },
                     0);
    lf->NextFunction(
        []() {
            return TransformValue(&TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y].second->scale, 0.0f,
                                  Game::Properties["rescale_speed"].f);
        },
        500);
    lf->NextFunction(
        []() {
            std::pair<int, MapTile *> &spawn_tile = TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y];
            spawn_tile.first = 0;
            delete spawn_tile.second;
            spawn_tile.second = nullptr;
            print("player created");
            return 1;
        },
        100);
    lf->NextFunction([]() {
        print("scene created");
        Game::player_won = false;
        return 1;
    });
    lf->Execute();
}

void Scene::DeleteScene()
{
    print("deleting current scene...");

    if (Game::player)
    {
        print("deleting player...");
        if (Game::Properties["sound"].b)
            PlaySound("win", CHANNEL_SPAWN_WIN, 0);
        LinkedFunction *lf = new LinkedFunction(
            []() { return TransformValue(&Game::player->scale, 0.0f, Game::Properties["rescale_speed"].f); });
        lf->NextFunction([]() {
            delete Game::player;
            Game::player = nullptr;
            Game::player_time[2] = SDL_GetTicks();
            print("player deleted");
            print("deleting tiles (without border)...");
            MapTile::DeleteTiles();
            return 1;
        });
        lf->NextFunction(
            []() {
                if (Game::scene)
                {
                    delete Game::scene;
                    Game::scene = nullptr;
                }
                print("tiles deleted");
                print("scene deleted");
                return 1;
            },
            (MapTile::nEmptyTiles + 4) * Game::Properties["map_animation_delay"].f);
        lf->NextFunction([]() {
            if (nextMap)
                Game::scene = new Scene(true);
            else
                Game::scene = new Scene();
            return 1;
        });
        lf->Execute();
    }
    else
    {
        delete this;
    }
}