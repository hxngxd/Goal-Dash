#include "game.h"
#include "gameobject.h"

Scene::Scene()
{
    ShowMsg(0, normal, "creating new scene...");
    MapTile::CreateBorder();

    auto start = []() {
        scene->DeleteScene();
        auto new_scene = []() {
            scene = new Scene(Game::Properties["map"].i);
            return 1;
        };
        DelayFunction::Create(500, new_scene);
        return 1;
    };
    Button::CreateButton("start", Screen::resolution / 2 - Vector2(0, 75), "Start", start);

    Button::CreateButton("settings", Screen::resolution / 2, "Settings");

    auto exit = []() {
        DelayFunction::Create(500, []() {
            game->Stop();
            return 1;
        });
    };
    Button::CreateButton("exit", Screen::resolution / 2 + Vector2(0, 75), "Exit", exit);

    ShowMsg(1, success, "done.");
}

Scene::Scene(int map)
{
    ShowMsg(0, normal, "creating new scene...");
    float wait = MapTile::CreateTiles(map);

    DelayFunction::Create(wait + 250, []() {
        ShowMsg(1, normal, "creating player...");
        playSound("spawn", spawn_win_channel, 0);

        Vector2 player_position(MapTile::SpawnTile.y * Screen::tile_size, MapTile::SpawnTile.x * Screen::tile_size);
        player = new Player(player_position);

        auto hide_spawn = []() {
            transformFValue(&TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y].second->scale, 0,
                            Game::Properties["rescale_speed"].f, 500, []() {
                                std::pair<int, MapTile *> &spawn_tile =
                                    TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y];
                                spawn_tile.first = 0;
                                delete spawn_tile.second;
                                spawn_tile.second = nullptr;
                            });
            ShowMsg(2, success, "done.");
        };

        transformFValue(&player->scale, 1, Game::Properties["rescale_speed"].f, 0, hide_spawn);

        Game::Properties["player_won"].b = 0;
        return 1;
    });

    ShowMsg(1, success, "done.");
}

void Scene::DeleteScene()
{
    ShowMsg(0, normal, "deleting current scene...");

    ShowMsg(1, normal, "deleting all UIs...");
    UI::DeleteUIs();
    ShowMsg(2, success, "done.");

    if (player)
    {
        ShowMsg(1, normal, "deleting player...");
        playSound("win", spawn_win_channel, 0);
        transformFValue(&player->scale, 0, Game::Properties["rescale_speed"].f, 0, []() {
            ShowMsg(2, success, "done.");
            ShowMsg(1, normal, "deleting all tiles...");
            float wait = MapTile::DeleteTiles();
            delete player;
            player = nullptr;
            DelayFunction::Create(
                wait + 1500,
                []() {
                    if (Game::scene)
                    {
                        delete Game::scene;
                        Game::scene = nullptr;
                    }
                    return 1;
                },
                []() { Game::scene = new Scene(++Game::Properties["map"].i); });
            ShowMsg(2, success, "done.");
        });
    }
    else
    {
        ShowMsg(1, normal, "deleting all tiles...");
        MapTile::DeleteTiles();
        ShowMsg(2, success, "done.");
        delete this;
    }
}