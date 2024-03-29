#include "ui.h"
#include "../datalib/mixer.h"
#include "../datalib/msg.h"
#include "../datalib/sprite.h"
#include "../datalib/util.h"
#include "../game.h"
#include "algorithm"
#include "input.h"

//----------------------------------------

std::map<std::string, Button *> Buttons;
std::map<std::string, Canvas *> Canvases;
std::string hoverButton = "", downButton = "", upButton = "";

//----------------------------------------

void UI::Start()
{
}

void UI::Update()
{
    if (!Canvases.empty())
    {
        for (auto &cv : Canvases)
        {
            if (cv.second)
                cv.second->Update();
        }
    }
}

//----------------------------------------

Button::Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label, int font_size,
               std::function<void()> onClick)
{
    print("creating", name, "button");
    Buttons[name] = this;
    this->name = name;
    this->position = position;
    this->size = size;
    this->scale = 1;
    this->label = label;
    this->bg_opacity = 0;
    this->label_opacity = 255;
    this->onClick = std::bind(
        [](std::function<void()> onClick) {
            onClick();
            PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
        },
        onClick);
    this->hovering_sound = this->button_mouse_hovering = this->button_mouse_click = false;
    this->lastButtonClick = 0;
    this->font_size = std::min(font_size, CalculateFontSize(size, label));

    print(name, "button created");
}

Button::Button(std::string name, std::string label, std::function<void()> onClick)
{
    print("creating", name, "button");
    Buttons[name] = this;
    this->name = name;
    this->position = Vector2();
    this->size = Vector2();
    this->scale = 1;
    this->label = label;
    this->bg_opacity = 0;
    this->label_opacity = 255;
    this->onClick = std::bind(
        [](std::function<void()> onClick) {
            onClick();
            PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
        },
        onClick);
    this->hovering_sound = this->button_mouse_hovering = this->button_mouse_click = false;
    this->lastButtonClick = 0;

    print(name, "button created");
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

    int min_bg_opacity = 0, max_bg_opacity = 144, opacity_step = 16;
    if (button_mouse_hovering)
    {
        if (!hovering_sound)
        {
            PlaySound("hover", CHANNEL_BUTTON_HOVER, 0);
            hovering_sound = true;
        }

        if (bg_opacity < max_bg_opacity)
        {
            bg_opacity += opacity_step;
            bg_opacity = Clamp(bg_opacity, min_bg_opacity, max_bg_opacity);
        }
    }
    else
    {
        if (hovering_sound)
            hovering_sound = false;

        if (button_mouse_click)
            button_mouse_click = false;

        if (bg_opacity > min_bg_opacity)
        {
            bg_opacity -= opacity_step;
            bg_opacity = Clamp(bg_opacity, min_bg_opacity, max_bg_opacity);
        }
    }

    if (bg_opacity)
    {
        Screen::SetDrawColor(Color::gray(64, bg_opacity - (button_mouse_click ? 64 : 0)));
        SDL_RenderFillRect(Game::renderer, &bgRect);
    }

    Screen::SetDrawColor(button_mouse_click ? Color::blue() : Color::white(64));
    SDL_RenderDrawRect(Game::renderer, &bgRect);

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}

void Button::DeleteButton(std::string name)
{
    if (Buttons.find(name) == Buttons.end())
        return;
    Button *&btn = Buttons[name];
    if (btn)
    {
        delete btn;
        btn = nullptr;
    }
    Buttons.erase(name);
    print("button", name, "deleted");
}

void Button::DeleteButtons()
{
    print("deleting buttons...");
    for (auto &btn : Buttons)
        DeleteButton(btn.first);
    print("buttons deleted");
    Buttons.clear();
}

//----------------------------------------

Canvas::Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing, int margin)
{
    print("creating", name, "canvas");
    Canvases[name] = this;
    this->name = name;
    this->position = position;
    this->size = size;
    this->bg_opacity = bg_opacity;
    this->spacing = spacing;
    this->margin = margin;

    print(name, "canvas created");
}

void Canvas::AddComponents(std::string type, std::string name)
{
    Components.push_back(std::make_pair(type, name));
    RecalculateComponentsPosition();
}

void Canvas::RemoveComponents(std::string type, std::string name)
{
    std::pair<std::string, std::string> p = std::make_pair(type, name);
    Components.erase(std::remove(Components.begin(), Components.end(), p), Components.end());
    RecalculateComponentsPosition();
}

void Canvas::RecalculateComponentsPosition()
{
    int numOfComponents = Components.size();
    if (!numOfComponents)
        return;
    int numOfSpacing = numOfComponents - 1;
    Vector2 ComponentSize =
        Vector2(size.x - 2 * margin, (size.y - 2 * margin - numOfSpacing * spacing) / numOfComponents);
    Vector2 currentPosition = position + Vector2(margin);
    if (!numOfComponents)
        return;
    for (auto &com : Components)
    {
        UI *ui = nullptr;
        if (com.first == "btn")
        {
            if (Buttons.find(com.second) != Buttons.end() && Buttons[com.second])
                ui = (UI *)Buttons[com.second];
        }
        if (ui)
        {
            ui->position = currentPosition;
            ui->size = ComponentSize;
            ui->font_size = CalculateFontSize(ComponentSize, ui->label);
            currentPosition.y += ComponentSize.y + spacing;
        }
    }
}

void Canvas::Update()
{
    SDL_Rect rect;
    rect.x = position.x;
    rect.y = position.y;
    rect.w = size.x;
    rect.h = size.y;
    Screen::SetDrawColor(Color::black(bg_opacity));
    SDL_RenderFillRect(Game::renderer, &rect);
    Screen::SetDrawColor(Color::white(32));
    SDL_RenderDrawRect(Game::renderer, &rect);
    for (auto &com : Components)
    {
        if (com.first == "btn")
        {
            if (Buttons.find(com.second) != Buttons.end() && Buttons[com.second])
                Buttons[com.second]->Update();
        }
    }
}

void Canvas::DeleteCanvas(std::string name)
{
    if (Canvases.find(name) == Canvases.end())
        return;
    Canvas *&cv = Canvases[name];
    if (cv)
    {
        delete cv;
        cv = nullptr;
    }
    Canvases.erase(name);
    print("canvas", name, "deleted");
}

void Canvas::DeleteCanvases()
{
    print("deleting canvases...");
    for (auto &cv : Canvases)
        DeleteCanvas(cv.first);
    print("canvases deleted");
    Canvases.clear();
}

//----------------------------------------

int CalculateFontSize(const Vector2 &bg_size, std::string label)
{
    int perfectFs = 500;
    SDL_Rect rect;
    rect.w = rect.h = 100000;
    while (rect.w >= bg_size.x || rect.h >= bg_size.y)
    {
        TTF_SetFontSize(myFont, perfectFs);
        TTF_SizeText(myFont, label.c_str(), &rect.w, &rect.h);
        perfectFs -= 2;
    }
    return perfectFs;
}