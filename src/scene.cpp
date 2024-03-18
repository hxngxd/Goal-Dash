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
        DelayFunction::CreateDelayFunction(500, new_scene);
        return 1;
    };
    Button::CreateButton("start", Screen::resolution / 2, Color::cyan(127), "START", 50, Color::white(255), start);
    Button::CreateButton("exit", Screen::resolution / 2 + Vector2(0, 100), Color::cyan(127), "exit", 50,
                         Color::white(255), []() { game->Stop(); });

    ShowMsg(1, success, "done.");
}

Scene::Scene(int map)
{
    ShowMsg(0, normal, "creating new scene...");
    std::pair<Vector2, float> x = MapTile::CreateTiles(map);
    ShowMsg(0, normal, "creating player...");
    auto create_player = [](Vector2 position) {
        player = new Player(position * Screen::tile_size);
        GameObject::reScale(player, 1, 0, Game::Properties["rescale_speed"].f);
        Game::Properties["player_won"].b = 0;
        return 1;
    };
    DelayFunction::CreateDelayFunction(x.second + 250, std::bind(create_player, x.first));
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
        stopAllSound();
        GameObject::reScale(player, 0, 0, Game::Properties["rescale_speed"].f, []() {
            ShowMsg(1, normal, "deleting all tiles...");
            float wait = MapTile::DeleteTiles();
            delete player;
            player = nullptr;
            DelayFunction::CreateDelayFunction(
                wait + 1000,
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
    Buttons[name]->onClick = onClick;

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
            hoverButton = name;
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