#include "game.h"
#include "gameobject.h"

std::map<std::string, Button *> Buttons;
std::string hoverButton = "", downButton = "", upButton = "";

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
    Button::CreateButton("start", Screen::resolution / 2, Color::cyan(127), "START", 50, Color::white(255), start);

    auto exit = []() {
        DelayFunction::Create(500, []() {
            game->Stop();
            return 1;
        });
    };
    Button::CreateButton("exit", Screen::resolution / 2 + Vector2(0, 100), Color::cyan(127), "exit", 50,
                         Color::white(255), exit);

    ShowMsg(1, success, "done.");
}

Scene::Scene(int map)
{
    ShowMsg(0, normal, "creating new scene...");
    float wait = MapTile::CreateTiles(map);
    ShowMsg(0, normal, "creating player...");

    DelayFunction::Create(wait + 250, []() {
        playSound("spawn", spawn_win_channel, 0);

        Vector2 player_position(MapTile::SpawnTile.y * Screen::tile_size, MapTile::SpawnTile.x * Screen::tile_size);
        player = new Player(player_position);

        auto hide_spawn = []() {
            GameObject::reScale(TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y].second, 0, 500,
                                Game::Properties["rescale_speed"].f, []() {
                                    std::pair<int, MapTile *> &spawn_tile =
                                        TileMap[MapTile::SpawnTile.x][MapTile::SpawnTile.y];
                                    spawn_tile.first = 0;
                                    delete spawn_tile.second;
                                    spawn_tile.second = nullptr;
                                });
        };

        GameObject::reScale(player, 1, 0, Game::Properties["rescale_speed"].f, hide_spawn);

        Game::Properties["player_won"].b = 0;
        Game::Properties["player_score"].i = 0;
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
        GameObject::reScale(player, 0, 0, Game::Properties["rescale_speed"].f, []() {
            ShowMsg(1, normal, "deleting all tiles...");
            float wait = MapTile::DeleteTiles();
            delete player;
            player = nullptr;
            DelayFunction::Create(
                wait + 1500,
                []() {
                    delete Game::scene;
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

void Scene::Update()
{
    MapTile::Draw();
}

void UI::Update()
{
    for (auto &btn : Buttons)
    {
        if (btn.second)
            btn.second->Update();
    }
}

void UI::DeleteUIs()
{
    for (auto &btn : Buttons)
    {
        if (btn.second)
        {
            delete btn.second;
            btn.second = nullptr;
            ShowMsg(2, success, "deleted " + btn.first + " button!");
        }
    }
    Buttons.clear();
}

bool Button::CreateButton(std::string name, const Vector2 &position, SDL_Color bg_color, std::string label,
                          int font_size, SDL_Color font_color, std::function<void()> onClick)
{
    ShowMsg(2, normal, "creating " + name + " button...");
    Buttons[name] = new Button();
    if (!Buttons[name])
    {
        ShowMsg(3, fail, "failed to create button.");
        Buttons.erase(name);
        return 0;
    }

    Buttons[name]->name = name;
    Buttons[name]->position = position;
    Buttons[name]->scale = 1;
    Buttons[name]->bg_color = bg_color;
    Buttons[name]->label = label;
    Buttons[name]->font_size = font_size;
    Buttons[name]->font_color = font_color;
    Buttons[name]->onClick = std::bind(
        [](std::function<void()> onClick) {
            onClick();
            playSound("click", button_channel, 0);
        },
        onClick);

    ShowMsg(3, success, "done.");
    return 1;
}

void Button::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), font_color);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    SDL_Rect labelRect;
    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    labelRect.x = position.x - labelRect.w / 2;
    labelRect.y = position.y - labelRect.h / 2;

    SDL_Rect bgRect = labelRect;
    bgRect.x -= font_size / 5;
    bgRect.w += font_size / 5 * 2;

    bool isHovered = IsInRange(mousePosition.x, bgRect.x, bgRect.x + bgRect.w) &&
                     IsInRange(mousePosition.y, bgRect.y, bgRect.y + bgRect.h);

    if (isHovered)
    {
        if (hoverButton != name)
        {
            hoverButton = name;
            playSound("hover", button_channel, 0);
        }
        Screen::SetDrawColor(Color::black(100));
    }
    else
    {
        if (hoverButton == name)
            hoverButton = "";
        Screen::SetDrawColor(bg_color);
    }

    SDL_RenderFillRect(Game::renderer, &bgRect);

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}