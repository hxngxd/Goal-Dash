#include "game.h"
#include "gameobject.h"

std::map<std::string, Button *> Buttons;
std::string hoverButton = "", downButton = "", upButton = "";

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
        if (!btn.second)
            continue;
        delete btn.second;
        btn.second = nullptr;
        ShowMsg(2, success, "deleted " + btn.first + " button!");
    }
    Buttons.clear();
}

bool Button::CreateButton(std::string name, const Vector2 &position, std::string label, std::function<void()> onClick)
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
    Buttons[name]->label = label;
    Buttons[name]->font_size = 40;
    Buttons[name]->bg_opacity = 0;
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
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white(255));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    SDL_Rect labelRect;
    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    labelRect.x = position.x - labelRect.w / 2;
    labelRect.y = position.y - labelRect.h / 2;

    SDL_Rect bgRect = labelRect;
    bgRect.x = Screen::resolution.x / 3;
    bgRect.w = Screen::resolution.x / 3;

    bool isHovered = IsInRange(mousePosition.x, bgRect.x, bgRect.x + bgRect.w) &&
                     IsInRange(mousePosition.y, bgRect.y, bgRect.y + bgRect.h);

    if (isHovered)
    {
        if (hoverButton != name || bg_opacity == 0)
        {
            hoverButton = name;
            playSound("hover", button_channel, 0);
            DelayFunction::Remove(DFid[0]);
            DFid[0] = transformFValue(&bg_opacity, 255, 20, 0);
            DelayFunction::Remove(DFid[1]);
            DFid[1] = transformFValue(&font_size, 45, 0.5, 0);
        }
    }
    else
    {
        if (hoverButton == name || bg_opacity == 255)
        {
            hoverButton = "";
            DelayFunction::Remove(DFid[0]);
            DFid[0] = transformFValue(&bg_opacity, 0, 20, 0);
            DelayFunction::Remove(DFid[1]);
            DFid[1] = transformFValue(&font_size, 40, 0.5, 0);
        }
    }

    if (bg_opacity)
    {
        setTextureOpacity(Sprites["button_hover"]->texture, bg_opacity);
        DrawSprite(Sprites["button_hover"], Vector2(bgRect.x, bgRect.y), Vector2(bgRect.w, bgRect.h), scale, 0, 0);
    }

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}