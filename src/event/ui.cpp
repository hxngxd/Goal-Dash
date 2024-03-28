#include "ui.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../datalib/sprite.h"
#include "../datalib/util.h"
#include "../game.h"
#include "input.h"

std::map<std::string, Button *> Buttons;
std::map<std::string, Text *> Texts;
std::string hoverButton = "", downButton = "", upButton = "";

void UI::Start()
{
}

void UI::Update()
{
    if (!Buttons.empty())
    {
        for (auto &btn : Buttons)
        {
            if (!btn.second)
                continue;
            btn.second->Update();
        }
    }
}

bool Button::CreateButton(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
                          int font_size, std::function<void()> onClick)
{
    print("creating", name, "button");
    Buttons[name] = new Button();
    Button *&btn = Buttons[name];
    if (!btn)
    {
        print("failed to create button", name);
        Buttons.erase(name);
        return 0;
    }

    btn->name = name;
    btn->position = position;
    btn->size = size;
    btn->scale = 1;
    btn->label = label;
    btn->bg_opacity = 0;
    btn->label_opacity = 255;
    btn->onClick = std::bind(
        [](std::function<void()> onClick) {
            onClick();
            PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
        },
        onClick);
    btn->button_mouse_hovering = btn->button_mouse_click = false;
    btn->lastButtonClick = 0;

    int perfectFs = 500;
    SDL_Rect rect;
    rect.w = rect.h = 10000;
    while (rect.w >= size.x || rect.h >= size.y)
    {
        TTF_SetFontSize(myFont, perfectFs);
        TTF_SizeText(myFont, label.c_str(), &rect.w, &rect.h);
        perfectFs--;
    }
    btn->font_size = std::min(font_size, perfectFs);

    print(name, "button created");
    return 1;
}

void Button::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white(label_opacity));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    SDL_Rect labelRect;
    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    Vector2 center = Rect::GetCenter(position, size);
    labelRect.x = center.x - labelRect.w / 2;
    labelRect.y = center.y - labelRect.h / 2;

    SDL_Rect bgRect;
    bgRect.x = position.x;
    bgRect.y = position.y;
    bgRect.w = size.x;
    bgRect.h = size.y;

    button_mouse_hovering = InRange(mousePosition.x, bgRect.x, bgRect.x + bgRect.w) &&
                            InRange(mousePosition.y, bgRect.y, bgRect.y + bgRect.h);

    if (button_mouse_hovering)
    {
        if (bg_opacity < 160)
        {
            bg_opacity += 16;
            bg_opacity = Clamp(bg_opacity, 64, 160);
        }
    }
    else
    {
        if (button_mouse_click)
            button_mouse_click = false;

        if (bg_opacity > 0)
        {
            bg_opacity -= 16;
            bg_opacity = Clamp(bg_opacity, 64, 160);
        }
    }

    if (bg_opacity)
    {
        Screen::SetDrawColor(Color::gray(32, bg_opacity - (button_mouse_click ? 64 : 0)));
        SDL_RenderFillRect(Game::renderer, &bgRect);
    }

    if (button_mouse_click)
    {
        Screen::SetDrawColor(Color::white(255));
        SDL_RenderDrawRect(Game::renderer, &bgRect);
    }

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}

void Button::DeleteButtons()
{
    print("deleting buttons...");
    for (auto &btn : Buttons)
    {
        if (!btn.second)
            continue;
        delete btn.second;
        btn.second = nullptr;
        print("button", btn.first, "deleted");
    }
    print("buttons deleted");
    Buttons.clear();
}

// bool Text::CreateText(std::string name, const Vector2 &position, std::string label, int font_size)
// {
//     print("creating", name, "text");
//     Texts[name] = new Text();
//     if (!Texts[name])
//     {
//         print("failed to create text", name);
//         Texts.erase(name);
//         return 0;
//     }

//     Texts[name]->name = name;
//     Texts[name]->position = position;
//     Texts[name]->scale = 1;
//     Texts[name]->label = label;
//     Texts[name]->font_size = font_size;
//     Texts[name]->bg_opacity = 0;
//     Texts[name]->label_opacity = 255;

//     print(name, "text created");
//     return 1;
// }

// void Text::Update()
// {
//     TTF_SetFontSize(myFont, font_size);
//     SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white(label_opacity));

//     SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

//     SDL_Rect labelRect;
//     TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
//     labelRect.x = position.x - labelRect.w / 2;
//     labelRect.y = position.y - labelRect.h / 2;

//     SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

//     SDL_FreeSurface(sf);
//     SDL_DestroyTexture(texture);
// }

// void Text::DeleteTexts()
// {
//     print("deleting texts...");
//     for (auto &text : Texts)
//     {
//         if (text.first == "time")
//             continue;
//         if (!text.second)
//             continue;
//         delete text.second;
//         text.second = nullptr;
//         print("text", text.first, "deleted");
//     }
//     print("texts deleted");
//     Texts.clear();
// }