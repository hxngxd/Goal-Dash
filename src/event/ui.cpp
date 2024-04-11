#include "ui.h"
#include "../datalib/mixer.h"
#include "../datalib/sprite.h"
#include "../game.h"
#include "algorithm"
#include "input.h"

//----------------------------------------

std::map<std::string, UI *> UI::UIs;
std::vector<std::string> UI::WaitingForRemoval;

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
    //     20);
    // vcv->AddComponents({"exit", "home", "settings", "mute"});

    // Canvas *hcv2 = new Canvas("horizontalhub2", Vector2(0, Screen::tile_size * (Screen::map_size - 1)),
    //                           Vector2(0, Screen::tile_size), 0, 0, 0, 0, 0);
    // Text *health = new Text("health", "Health: " + str(Game::player_health), Vector2(Screen::tile_size * 3, 0), 25);
    // hcv2->AddComponents({"health"});
}

//----------------------------------------

void UI::Start()
{
    EventHandler::MouseDownActions.insert(std::make_pair("UIButtonMouseDown", []() {
        for (auto &ui : UIs)
        {
            if (!ui.second)
                continue;
            if (ui.second->type != BUTTON)
                continue;
            Button *btn = (Button *)ui.second;
            if (btn->button_mouse_hovering && btn->enabled)
                btn->button_mouse_click = true;
        }
    }));

    EventHandler::MouseUpActions.insert(std::make_pair("UIButtonMouseUp", []() {
        for (auto &ui : UIs)
        {
            if (!ui.second)
                continue;
            if (ui.second->type != BUTTON)
                continue;
            Button *btn = (Button *)ui.second;
            if (btn->button_mouse_click && btn->button_mouse_hovering &&
                (SDL_GetTicks() - btn->lastButtonClick >= 250) && btn->enabled)
            {
                btn->onClick();
                if (btn)
                    btn->lastButtonClick = SDL_GetTicks();
            }
            if (btn)
                btn->button_mouse_click = false;
        }
    }));
}

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

void UI::RemoveUI(std::string name)
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

void UI::RemoveUIs()
{
    if (WaitingForRemoval.empty())
        return;
    print("deleting uis...");
    for (auto &ui : WaitingForRemoval)
        RemoveUI(ui);
    WaitingForRemoval.clear();
    print("uis deleted");
}

void UI::RemovingUI(std::string name)
{
    WaitingForRemoval.push_back(name);
}

void UI::RemovingUIs()
{
    for (auto &ui : UIs)
        WaitingForRemoval.push_back(ui.first);
}

//----------------------------------------

Button::Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
               std::function<void()> onClick, int font_size)
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

Text::Text(std::string name, const Vector2 &position, const Vector2 &size, std::string label, int font_size)
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

Canvas::Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing, int margin,
               bool vertical)
    : UI(CANVAS, name, position, size, "", 0)
{
    print("creating", name, "canvas");
    UIs[name] = this;
    this->bg_opacity = bg_opacity;
    this->spacing = spacing;
    this->margin = margin;
    this->vertical = vertical;
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

        ui->size = size - Vector2(2.0f * margin);

        ui->font_size = std::min(ui->original_font_size, Text::CalculateFontSize(ui->size, ui->label));
        return;
    }

    int numOfBlocks = 0;
    for (auto &com : Components)
        numOfBlocks += com.second;

    int numOfSpacings = numOfBlocks - 1;

    float blockSize = ((vertical ? size.y : size.x) - 2.0f * margin - numOfSpacings * spacing) / (float)numOfBlocks;

    Vector2 currentPosition = position + Vector2(margin);

    for (auto &com : Components)
    {
        UI *&ui = UI::UIs[com.first];
        if (!ui)
            continue;

        ui->position = currentPosition;
        if (vertical)
        {
            ui->size.x = size.x - 2.0f * margin;
            ui->size.y = (float)spacing * (com.second - 1) + blockSize * com.second;
            currentPosition.y += ui->size.y + spacing;
        }
        else
        {
            ui->size.y = size.y - 2.0f * margin;
            ui->size.x = (float)spacing * (com.second - 1) + blockSize * com.second;
            currentPosition.x += ui->size.x + spacing;
        }
        ui->font_size = std::min(ui->original_font_size, Text::CalculateFontSize(ui->size, ui->label));
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