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
        }
    }
    for (auto &ui : UIs)
    {
        if (ui.second)
        {
            if (ui.second->type == BUTTON)
            {
                Button *btn = (Button *)ui.second;
                btn->Update();
            }
            else if (ui.second->type == TEXT)
            {
                Text *txt = (Text *)ui.second;
                txt->Update();
            }
            else if (ui.second->type == SLIDER)
            {
                Slider *sldr = (Slider *)ui.second;
                sldr->Update();
            }
        }
    }
}

UI::UI(int type, std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
       int font_size)
{
    this->type = type;
    this->name = name;
    this->position = position;
    this->size = size;
    this->bg_opacity = 0;
    this->border_opacity = 127;
    this->label = label;
    this->font_size = this->original_font_size = font_size;
    this->label_alignment = label_alignment;
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
    : UI(BUTTON, name, position, size, label, 0, font_size)
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

void Button::Recalculate()
{
    font_size = std::min(original_font_size, Text::CalculateFontSize(size, label));

    TTF_SetFontSize(myFont, font_size);
    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    Vector2 center = Rect::GetCenter(position, size);
    labelRect.x = center.x - labelRect.w / 2.0f;
    labelRect.y = center.y - labelRect.h / 2.0f;

    bgRect.x = position.x;
    bgRect.y = position.y;
    bgRect.w = size.x;
    bgRect.h = size.y;
}

//----------------------------------------

Text::Text(std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
           int font_size)
    : UI(TEXT, name, position, size, label, label_alignment, font_size)
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

    if (bg_opacity)
    {
        Screen::SetDrawColor(Color::gray(16, bg_opacity));
        SDL_RenderFillRect(Game::renderer, &bgRect);
    }

    Screen::SetDrawColor(Color::white(64));
    SDL_RenderDrawRect(Game::renderer, &bgRect);

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}

void Text::Recalculate()
{
    font_size = std::min(original_font_size, Text::CalculateFontSize(size, label));

    TTF_SetFontSize(myFont, font_size);
    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    Vector2 center = Rect::GetCenter(position, size);
    int indent = Screen::resolution.x / 95.0f;
    switch (label_alignment)
    {
    case 0:
        labelRect.x = position.x + indent;
        break;
    case 1:
        labelRect.x = center.x - labelRect.w / 2.0f;
        break;
    case 2:
        labelRect.x = position.x + size.x - labelRect.w - indent;
        break;
    }
    labelRect.y = center.y - labelRect.h / 2.0f;

    bgRect.x = position.x;
    bgRect.y = position.y;
    bgRect.w = size.x;
    bgRect.h = size.y;
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

void Text::SetLabel(std::string name, std::string label)
{
    if (UIs.find(name) != UIs.end())
        UIs[name]->label = label;
}

//----------------------------------------

Slider::Slider(std::string name, const Vector2 &position, const Vector2 &size, float min_value, float max_value,
               float current_value, float step, int font_size)
    : UI(SLIDER, name, position, size, "", 0, font_size)
{
    print("creating", name, "slider");
    UIs[name] = this;
    this->min_value = min_value;
    this->max_value = max_value;
    this->current_value = current_value;
    this->step = step;
    print(name, "slider created");
}

void Slider::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *min_sf = TTF_RenderText_Blended(myFont, strRound(min_value, 1).c_str(), Color::white());
    SDL_Texture *min_texture = SDL_CreateTextureFromSurface(Game::renderer, min_sf);

    SDL_Surface *max_sf = TTF_RenderText_Blended(myFont, strRound(max_value, 1).c_str(), Color::white());
    SDL_Texture *max_texture = SDL_CreateTextureFromSurface(Game::renderer, max_sf);

    SDL_RenderCopy(Game::renderer, min_texture, NULL, &minRect);
    SDL_RenderCopy(Game::renderer, max_texture, NULL, &maxRect);

    Screen::SetDrawColor(Color::white());
    SDL_RenderFillRect(Game::renderer, &barRect);

    Screen::SetDrawColor(Color::white(64));
    SDL_RenderDrawRect(Game::renderer, &bgRect);

    // SDL_Rect btnRect;
    // btnRect.x = Clamp(EventHandler::MousePosition.x, (float)barRect.x, (float)(barRect.x + barRect.w));
    // btnRect.y = barRect.y;
    // btnRect.w = btnRect.h = barRect.h;
    // DrawSprite("circ", Rect::GetPosition(btnRect), Rect::GetSize(btnRect), 1.0f, 0, 0);

    SDL_FreeSurface(min_sf);
    SDL_DestroyTexture(min_texture);
    SDL_FreeSurface(max_sf);
    SDL_DestroyTexture(max_texture);
}

void Slider::Recalculate()
{
    min_position.x = position.x + size.x / 5.0f;
    min_position.y = position.y + size.y / 2.0f - 2.5f;

    max_position.x = position.x + size.x * 4.0f / 5.0f;
    max_position.y = position.y + size.y / 2.0f + 2.5f;

    barRect.x = min_position.x;
    barRect.y = min_position.y;
    barRect.w = max_position.x - min_position.x;
    barRect.h = max_position.y - min_position.y;

    minRect.w = min_position.x - position.x;
    minRect.h = 50.0f;
    minRect.x = position.x;
    minRect.y = position.y + size.y / 2.0f - minRect.h / 2.0f;

    Vector2 minCenter = Rect::GetCenter(Vector2(minRect.x, minRect.y), Vector2(minRect.w, minRect.h));

    maxRect.w = minRect.w;
    maxRect.h = minRect.h;
    maxRect.x = max_position.x;
    maxRect.y = minRect.y;

    Vector2 maxCenter = Rect::GetCenter(Vector2(maxRect.x, maxRect.y), Vector2(maxRect.w, maxRect.h));

    font_size =
        std::min(original_font_size, Text::CalculateFontSize(Vector2(minRect.w, minRect.h), strRound(min_value, 1)));

    TTF_SetFontSize(myFont, font_size);
    TTF_SizeText(myFont, strRound(min_value, 1).c_str(), &minRect.w, &minRect.h);

    minRect.x = minCenter.x - minRect.w / 2.0f;
    minRect.y = minCenter.y - minRect.h / 2.0f;

    font_size =
        std::min(original_font_size, Text::CalculateFontSize(Vector2(maxRect.w, maxRect.h), strRound(max_value, 1)));

    TTF_SetFontSize(myFont, font_size);
    TTF_SizeText(myFont, strRound(max_value, 1).c_str(), &maxRect.w, &maxRect.h);

    maxRect.x = maxCenter.x - maxRect.w / 2.0f;
    maxRect.y = maxCenter.y - maxRect.h / 2.0f;

    bgRect.x = position.x;
    bgRect.y = position.y;
    bgRect.w = size.x;
    bgRect.h = size.y;
}

void Slider::SetValue(std::string name, float value)
{
    if (UIs.find(name) != UIs.end())
        ((Slider *)UIs[name])->current_value = value;
}

//----------------------------------------

Canvas::Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing, int margin,
               bool vertical)
    : UI(CANVAS, name, position, size, "", 0, 0)
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
        if (UI::UIs.find(Components[0].first) == UI::UIs.end())
            return;

        UI *&ui = UIs[Components[0].first];
        ui->position = position + Vector2(margin);

        ui->size = size - Vector2(2.0f * margin);

        switch (ui->type)
        {
        case BUTTON:
            ((Button *)ui)->Recalculate();
            break;
        case TEXT:
            ((Text *)ui)->Recalculate();
            break;
        case SLIDER:
            ((Slider *)ui)->Recalculate();
            break;
        }
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
        if (UI::UIs.find(com.first) == UI::UIs.end())
            continue;
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
        switch (ui->type)
        {
        case BUTTON:
            ((Button *)ui)->Recalculate();
            break;
        case TEXT:
            ((Text *)ui)->Recalculate();
            break;
        case SLIDER:
            ((Slider *)ui)->Recalculate();
            break;
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
}