#include "ui.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../datalib/sprite.h"
#include "../datalib/util.h"
#include "../game.h"
#include "input.h"

std::map<std::string, Button *> Buttons;
std::string hoverButton = "", downButton = "", upButton = "";
int normalFS = 38, hoverFS = normalFS + 3, clickFS = hoverFS + 3, lastClicked = 0;
std::map<std::string, Text *> Texts;
int createTime = 0, startTime = 0, stopTime = 0;

void UI::Update()
{
    if (!Buttons.empty())
    {
        for (auto &btn : Buttons)
        {
            if (btn.second)
                btn.second->Update();
        }
    }

    if (Texts.find("time") != Texts.end())
    {
        DrawSprite("card1", Vector2(Screen::resolution.x / 2 - Screen::tile_size * 3, 0),
                   Vector2(Screen::tile_size * 6, Screen::tile_size), 1, 0);
        Texts["time"]->Update();
    }
    if (!Texts.empty())
    {
        for (auto &text : Texts)
        {
            if (text.second)
                text.second->Update();
        }
    }
}

bool Button::CreateButton(std::string name, const Vector2 &position, std::string label, std::function<void()> onClick)
{
    print("creating", name, "button");
    Buttons[name] = new Button();
    if (!Buttons[name])
    {
        print("failed to create button", name);
        Buttons.erase(name);
        return 0;
    }

    Buttons[name]->name = name;
    Buttons[name]->position = position;
    Buttons[name]->scale = 1;
    Buttons[name]->label = label;
    Buttons[name]->font_size = normalFS;
    Buttons[name]->bg_opacity = 0;
    Buttons[name]->label_opacity = 255;
    Buttons[name]->onClick = std::bind(
        [](std::function<void()> onClick, std::string name) {
            LinkedFunction *lf =
                new LinkedFunction(std::bind(TransformValue<int>, &Buttons[name]->font_size, clickFS, 1));
            lf->NextFunction(std::bind(TransformValue<int>, &Buttons[name]->font_size, hoverFS, 1));
            lf->NextFunction(std::bind(
                                 [](std::function<void()> onClick, std::string name) {
                                     onClick();
                                     return 1;
                                 },
                                 onClick, name),
                             100);
            lf->Execute();
            PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
        },
        onClick, name);

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
    labelRect.x = position.x - labelRect.w / 2;
    labelRect.y = position.y - labelRect.h / 2;

    SDL_Rect bgRect = labelRect;
    bgRect.x = Screen::resolution.x / 3 - Screen::tile_size;
    bgRect.w = Screen::resolution.x / 3 + Screen::tile_size * 2;

    bool isHovered = InRange(mousePosition.x, bgRect.x, bgRect.x + bgRect.w) &&
                     InRange(mousePosition.y, bgRect.y, bgRect.y + bgRect.h);

    if (isHovered)
    {
        if (bg_opacity < 255)
        {
            bg_opacity += 17;
            bg_opacity = Clamp(bg_opacity, 0, 255);
        }
        if (font_size < hoverFS)
        {
            font_size++;
            font_size = Clamp(font_size, normalFS, hoverFS);
        }
        if (hoverButton != name)
            PlaySound("hover", CHANNEL_BUTTON_HOVER, 0);

        hoverButton = name;
    }
    else
    {
        if (bg_opacity > 0)
        {
            bg_opacity -= 17;
            bg_opacity = Clamp(bg_opacity, 0, 255);
        }
        if (font_size > normalFS)
        {
            font_size--;
            font_size = Clamp(font_size, normalFS, hoverFS);
        }
        if (hoverButton == name)
            hoverButton = "";
    }

    if (bg_opacity)
    {
        SetSpriteOpacity("card", bg_opacity);
        DrawSprite("card", Vector2(bgRect.x, bgRect.y), Vector2(bgRect.w, bgRect.h), scale, 0, 0);
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

bool Text::CreateText(std::string name, const Vector2 &position, std::string label, int font_size)
{
    print("creating", name, "text");
    Texts[name] = new Text();
    if (!Texts[name])
    {
        print("failed to create text", name);
        Texts.erase(name);
        return 0;
    }

    Texts[name]->name = name;
    Texts[name]->position = position;
    Texts[name]->scale = 1;
    Texts[name]->label = label;
    Texts[name]->font_size = font_size;
    Texts[name]->bg_opacity = 0;
    Texts[name]->label_opacity = 255;

    print(name, "text created");
    return 1;
}

void Text::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white(label_opacity));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    SDL_Rect labelRect;
    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    labelRect.x = position.x - labelRect.w / 2;
    labelRect.y = position.y - labelRect.h / 2;

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}

void Text::DeleteTexts()
{
    print("deleting texts...");
    for (auto &text : Texts)
    {
        if (text.first == "time")
            continue;
        if (!text.second)
            continue;
        delete text.second;
        text.second = nullptr;
        print("text", text.first, "deleted");
    }
    print("texts deleted");
    Texts.clear();
}