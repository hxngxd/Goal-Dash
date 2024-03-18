#include "game.h"
#include "gameobject.h"

std::map<std::string, Button *> Buttons;
std::string hoverButton = "", downButton = "", upButton = "";

Scene::Scene()
{
    ShowMsg(0, normal, "creating new scene...");
    MapTile::CreateBorder();

    auto start = [](Scene *scene) {
        scene->DeleteScene();
        auto new_scene = [](Scene *scene) {
            scene = new Scene("1");
            return 1;
        };
        DelayFunction::CreateDelayFunction(1000, std::bind(new_scene, scene));
        return 1;
    };
    Button::CreateButton("start", Screen::resolution / 2, Color::cyan(127), "START", 50, Color::white(255),
                         std::bind(start, scene));
    Button::CreateButton("exit", Screen::resolution / 2 + Vector2(0, 100), Color::cyan(127), "exit", 50,
                         Color::white(255), []() { game->Stop(); });

    ShowMsg(1, success, "done.");
}

Scene::Scene(std::string map)
{
    ShowMsg(0, normal, "creating new scene...");
    MapTile::CreateTiles(map);
    ShowMsg(1, success, "done.");
}

void Scene::DeleteScene()
{
    ShowMsg(0, normal, "deleting current scene...");

    ShowMsg(1, normal, "deleting all tiles...");
    MapTile::DeleteTiles();
    ShowMsg(2, success, "done.");

    ShowMsg(1, normal, "deleting all UIs...");
    UI::DeleteUIs();
    ShowMsg(2, success, "done.");

    delete this;
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