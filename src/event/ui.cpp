#include "ui.h"
#include "../datalib/mixer.h"
#include "../datalib/sprite.h"
#include "../game.h"
#include "algorithm"
#include "input.h"

//----------------------------------------

std::map<std::string, UI *> UI::UIs;

//----------------------------------------

void PlayerHUD()
{
    // Canvas *hcv1 = new Canvas("horizontalhub1", Vector2(), Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    // Text *score = new Text("score", "Score: " + str(Game::player_score), Vector2(Screen::tile_size * 3, 0), 25);
    // Text *time = new Text("time", "Time: 00:00:00.000", Vector2(Screen::tile_size * 5, 0), 25);
    // Text *map = new Text("map", "Map: 0", Vector2(Screen::tile_size * 2, 0), 25);
    // Text *difficulty = new Text("difficulty", "Difficulty: Easy", Vector2(Screen::tile_size * 5, 0), 25);
    // hcv1->AddComponents({"score", "time", "map", "difficulty"});

    // Canvas *vcv = new Canvas("verticalhub", Vector2(Screen::tile_size * (Screen::map_size - 1), 0),
    //                          Vector2(Screen::tile_size, Screen::tile_size * 4), 0, 0, 0);
    // Button *exitbtn = new Button(
    //     "exit", "Exit", Vector2(), []() { game->Stop(); }, 20);
    // Button *homebtn = new Button(
    //     "home", "Home", Vector2(),
    //     []() {
    //         nextMap = false;
    //         Game::scene->DeleteScene();
    //     },
    //     20);
    // Button *settingsbtn = new Button(
    //     "settings", "Settings", Vector2(), []() {}, 20);
    // Button *mutebtn = new Button(
    //     "mute", "Mute", Vector2(),
    //     []() {
    //         bool &msc = Game::Properties["music"].b;
    //         bool &snd = Game::Properties["sound"].b;
    //         if (msc)
    //             Mix_PauseMusic();
    //         else
    //             Mix_ResumeMusic();
    //         if (snd)
    //             StopAllSound();
    //         msc = !msc;
    //         snd = !snd;
    //     },
    //     20);
    // vcv->AddComponents({"exit", "home", "settings", "mute"});

    // Canvas *hcv2 = new Canvas("horizontalhub2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
    //                           Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    // Text *health = new Text("health", "Health: " + str(Game::player_health), Vector2(Screen::tile_size * 3, 0), 25);
    // hcv2->AddComponents({"health"});
}

//----------------------------------------

void UI::Update()
{
    for (auto &ui : UIs)
    {
        if (ui.second)
        {
            if (ui.second->type == CANVAS)
            {
                Canvas *cv = (Canvas *)ui.second;
                cv->Update();
            }
            else if (ui.second->type == BUTTON)
            {
                Button *btn = (Button *)ui.second;
                btn->Update();
            }
            else if (ui.second->type == TEXT)
            {
                Text *txt = (Text *)ui.second;
                txt->Update();
            }
        }
    }
}

UI::UI(int type, std::string name, const Vector2 &position, const Vector2 &size, std::string label, int font_size)
{
    this->type = type;
    this->name = name;
    this->position = position;
    this->size = size;
    this->bg_opacity = 0;
    this->border_opacity = 127;
    this->label = label;
    this->font_size = this->original_font_size = font_size;
}

void UI::DeleteUI(std::string name)
{
    if (UIs.find(name) == UIs.end())
        return;
    UI *&ui = UIs[name];
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
    UIs.erase(name);
    print("ui", name, "deleted");
}

void UI::DeleteUIs()
{
    if (UIs.empty())
        return;
    print("deleting uis...");
    for (auto &ui : UIs)
        DeleteUI(ui.first);
    print("uis deleted");
    UIs.clear();
}

//----------------------------------------

Button::Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
               std::function<void()> onClick, int font_size = 150)
    : UI(BUTTON, name, position, size, label, font_size)
{
    print("creating", name, "button");
    UIs[name] = this;
    this->onClick = std::bind(
        [](std::function<void()> onClick) {
            onClick();
            PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
        },
        onClick);
    this->hovering_sound = this->button_mouse_hovering = this->button_mouse_click = false;
    this->lastButtonClick = 0;
    this->enabled = true;

    print(name, "button created");
}

void Button::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white());

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

    button_mouse_hovering = InRange(EventHandler::MousePosition.x, bgRect.x, bgRect.x + bgRect.w) &&
                            InRange(EventHandler::MousePosition.y, bgRect.y, bgRect.y + bgRect.h);

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

//----------------------------------------

Text::Text(std::string name, const Vector2 &position, Vector2 &size, std::string label, int font_size = 150)
    : UI(TEXT, name, position, size, label, font_size)
{
    print("creating", name, "text");
    UIs[name] = this;
    print(name, "text created");
}

void Text::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white());

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

    if (bg_opacity)
    {
        Screen::SetDrawColor(Color::gray(16, bg_opacity));
        SDL_RenderFillRect(Game::renderer, &bgRect);
    }

    Screen::SetDrawColor(Color::white(127));
    SDL_RenderDrawRect(Game::renderer, &bgRect);

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}

int Text::CalculateFontSize(const Vector2 &bg_size, std::string label)
{
    int perfectFs = 500;
    SDL_Rect rect;
    rect.w = rect.h = 100000;
    while ((rect.w >= bg_size.x || rect.h >= bg_size.y) && perfectFs >= 10)
    {
        TTF_SetFontSize(myFont, perfectFs);
        TTF_SizeText(myFont, label.c_str(), &rect.w, &rect.h);
        perfectFs -= 2;
    }
    return perfectFs;
}

//----------------------------------------

Canvas::Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing = 0,
               int margin = 0, bool vertical = true, bool fixed_size = true)
    : UI(CANVAS, name, position, size, "", 0)
{
    print("creating", name, "canvas");
    UIs[name] = this;
    this->spacing = spacing;
    this->margin = margin;
    this->vertical = vertical;
    this->fixed_size = fixed_size;
    print(name, "canvas created");
}

void Canvas::AddComponent(std::string name, int blocks)
{
    Components.push_back(std::make_pair(name, blocks));
    CalculateComponentsPosition();
}

void Canvas::AddComponents(const std::vector<std::pair<std::string, int>> &names)
{
    for (auto &name : names)
        Components.push_back(name);
    CalculateComponentsPosition();
}

void Canvas::RemoveComponent(std::string name)
{
    int i = 0;
    for (; i < Components.size(); i++)
    {
        if (Components[i].first == name)
            break;
    }
    Components.erase(Components.begin() + i);
    CalculateComponentsPosition();
}

void Canvas::CalculateComponentsPosition()
{
    int numOfComponents = Components.size();
    if (!numOfComponents)
        return;

    if (numOfComponents == 1)
    {
        UI *&ui = UIs[Components[0].first];
        ui->position = position + Vector2(margin);
        if (fixed_size)
            ui->size = size - Vector2(2 * margin);
        else
        {
            if (vertical_alignment)
            {
                ui->size.x = size.x - 2 * margin;
                size.y = ui->size.y + 2 * margin;
            }
            else
            {
                ui->size.y = size.y - 2 * margin;
                size.x = ui->size.x + 2 * margin;
            }
        }
        ui->font_size = std::min(ui->original_font_size, CalculateFontSize(ui->size, ui->label));
        return;
    }

    int numOfSpacing = numOfComponents - 1;
    Vector2 currentPosition = position + Vector2(margin);

    if (fixed_size)
    {
        Vector2 ComponentSize = Vector2();
        if (vertical_alignment)
        {
            ComponentSize =
                Vector2(size.x - 2 * margin, ceil((size.y - 2 * margin - numOfSpacing * spacing) / numOfComponents));
        }
        else
        {
            ComponentSize =
                Vector2(ceil((size.x - 2 * margin - numOfSpacing * spacing) / numOfComponents), size.y - 2 * margin);
        }

        for (auto &com : Components)
        {
            UI *&ui = UIs[com];
            if (!ui)
                continue;
            ui->size = ComponentSize;
            ui->position = currentPosition;
            if (vertical_alignment)
                currentPosition.y += ComponentSize.y + spacing;
            else
                currentPosition.x += ComponentSize.x + spacing;
            ui->font_size = std::min(ui->original_font_size, CalculateFontSize(ui->size, ui->label));
        }
    }
    else
    {
        if (vertical_alignment)
            size.y = 2 * margin;
        else
            size.x = 2 * margin;
        for (auto &com : Components)
        {
            UI *&ui = UIs[com];
            if (!ui)
                continue;
            ui->position = currentPosition;
            if (vertical_alignment)
            {
                currentPosition.y += ui->size.y + spacing;
                size.y += ui->size.y + spacing;
                ui->size.x = size.x - 2 * margin;
            }
            else
            {
                currentPosition.x += ui->size.x + spacing;
                size.x += ui->size.x + spacing;
                ui->size.y = size.y - 2 * margin;
            }
            ui->font_size = std::min(ui->original_font_size, CalculateFontSize(ui->size, ui->label));
        }
        if (vertical_alignment)
            size.y -= spacing;
        else
            size.x -= spacing;
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
}