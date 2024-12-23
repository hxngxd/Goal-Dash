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
            if (!ui.second || !ui.second->visible)
                continue;
            if (ui.second->type == BUTTON)
            {
                Button *btn = (Button *)ui.second;
                if (btn->mouse_hovering && btn->enabled)
                    btn->mouse_click = true;
            }
            else if (ui.second->type == SLIDER)
            {
                Slider *sldr = (Slider *)ui.second;
                if (sldr->mouse_hovering)
                    sldr->is_focus = true;
            }
            else if (ui.second->type == TOGGLE)
            {
                Toggle *tg = (Toggle *)ui.second;
                if (tg->mouse_hovering)
                    tg->mouse_click = true;
            }
        }
    }));

    EventHandler::MouseUpActions.insert(std::make_pair("UIButtonMouseUp", []() {
        for (auto &ui : UIs)
        {
            if (!ui.second || !ui.second->visible)
                continue;
            if (ui.second->type == BUTTON)
            {
                Button *btn = (Button *)ui.second;
                if (btn->mouse_click && btn->mouse_hovering && (SDL_GetTicks() - btn->lastButtonClick >= 250) &&
                    btn->enabled)
                {
                    btn->onClick();
                    if (Game::properties["sound"].b)
                        PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
                    btn->lastButtonClick = SDL_GetTicks();
                }
                btn->mouse_click = false;
            }
            else if (ui.second->type == SLIDER)
            {
                Slider *sldr = (Slider *)ui.second;
                sldr->is_focus = false;
            }
            else if (ui.second->type == TOGGLE)
            {
                Toggle *tg = (Toggle *)ui.second;
                if (tg->mouse_click && tg->mouse_hovering)
                {
                    tg->option = !tg->option;
                    tg->onSwitch(tg->option);
                    if (Game::properties["sound"].b)
                        PlaySound("click", CHANNEL_BUTTON_CLICK, 0);
                }
                tg->mouse_click = false;
            }
        }
    }));
}

void UI::Update()
{
    for (auto &ui : UIs)
    {
        if (!ui.second || !ui.second->visible || ui.second->name == "msgbg" || ui.second->name == "msg")
            continue;
        if (ui.second->type == CANVAS)
        {
            Canvas *cv = (Canvas *)ui.second;
            cv->Update();
        }
    }
    for (auto &ui : UIs)
    {
        if (!ui.second || !ui.second->visible || ui.second->name == "msgbg" || ui.second->name == "msgbg.msg")
            continue;
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
        else if (ui.second->type == TOGGLE)
        {
            Toggle *tg = (Toggle *)ui.second;
            tg->Update();
        }
    }

    if (UIs.find("msgbg") != UIs.end())
        ((Canvas *)UIs["msgbg"])->Update();
    if (UIs.find("msgbg.msg") != UIs.end())
        ((Text *)UIs["msgbg.msg"])->Update();
}

UI::UI(int type, std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
       int font_size, int border_opacity)
{
    this->type = type;
    this->name = name;
    this->position = position;
    this->size = size;
    this->bg_opacity = 0;
    this->border_opacity = border_opacity;
    this->label = label;
    this->font_size = this->original_font_size = font_size;
    this->label_alignment = label_alignment;
    this->visible = true;
}

void UI::RemoveUI(std::string name)
{
    if (UIs.find(name) == UIs.end())
        return;
    UI *&ui = UIs[name];
    if (ui)
    {
        if (ui->type == CANVAS)
        {
            Canvas *cv = (Canvas *)ui;
            for (auto &com : cv->Components)
                RemoveUI(com.first);
        }
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

void UI::SetVisible(std::string name, bool visible)
{
    if (UIs.find(name) != UIs.end())
    {
        if (UIs[name]->visible != visible)
            Recalculate(UIs[name], visible);
    }
}

void UI::Recalculate(UI *ui, bool visible)
{
    if (ui->visible != visible)
        ui->visible = visible;
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
    case TOGGLE:
        ((Toggle *)ui)->Recalculate();
        break;
    case CANVAS:
        ((Canvas *)ui)->Recalculate();
        break;
    }
}

void UI::Recalculate(std::string name)
{
    Recalculate(UI::UIs[name], UI::UIs[name]->visible);
}

//----------------------------------------

Button::Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
               std::function<void()> onClick, int font_size, int border_opacity)
    : UI(BUTTON, name, position, size, label, 0, font_size, border_opacity)
{
    print("creating", name, "button");
    UIs[name] = this;
    this->onClick = onClick;
    this->hovering_sound = this->mouse_hovering = this->mouse_click = false;
    this->lastButtonClick = 0;
    this->enabled = true;
    this->selected = false;

    print(name, "button created");
}

void Button::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white());

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    mouse_hovering = InRange(EventHandler::MousePosition.x, bgRect.x, bgRect.x + bgRect.w) &&
                     InRange(EventHandler::MousePosition.y, bgRect.y, bgRect.y + bgRect.h);

    int min_bg_opacity = 0, max_bg_opacity = 144, opacity_step = 16;
    if (mouse_hovering)
    {
        if (!hovering_sound)
        {
            if (Game::properties["sound"].b)
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

        if (mouse_click)
            mouse_click = false;

        if (bg_opacity > min_bg_opacity)
        {
            bg_opacity -= opacity_step;
            bg_opacity = Clamp(bg_opacity, min_bg_opacity, max_bg_opacity);
        }
    }

    if (bg_opacity)
    {
        Screen::SetDrawColor(Color::gray(64, bg_opacity - (mouse_click ? 64 : 0)));
        SDL_RenderFillRect(Game::renderer, &bgRect);
    }

    Screen::SetDrawColor(mouse_click ? Color::blue() : (selected ? Color::green() : Color::white(border_opacity)));
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
           int font_size, int border_opacity, bool caret)
    : UI(TEXT, name, position, size, label, label_alignment, font_size, border_opacity)
{
    print("creating", name, "text");
    UIs[name] = this;
    this->caret = caret;
    if (this->caret)
    {
        toggle_caret = true;
        caret_opacity = 0;
    }
    print(name, "text created");
}

void Text::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *sf = TTF_RenderText_Blended(myFont, label.c_str(), Color::white());

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    TTF_SizeText(myFont, label.c_str(), &labelRect.w, &labelRect.h);
    Vector2 center = Rect::GetCenter(position, size);
    int indent = Screen::resolution.x / 40.0f;
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

    if (caret)
    {
        if (toggle_caret)
        {
            caret_opacity += 8;
            if (caret_opacity >= 248)
                toggle_caret = false;
        }
        else
        {
            caret_opacity -= 8;
            if (caret_opacity <= 8)
                toggle_caret = true;
        }
        Screen::SetDrawColor(Color::white(caret_opacity));
        SDL_RenderDrawLine(Game::renderer, labelRect.x + labelRect.w, labelRect.y, labelRect.x + labelRect.w,
                           labelRect.y + labelRect.h);
    }

    Screen::SetDrawColor(Color::white(border_opacity));
    SDL_RenderDrawRect(Game::renderer, &bgRect);

    SDL_RenderCopy(Game::renderer, texture, NULL, &labelRect);

    SDL_FreeSurface(sf);
    SDL_DestroyTexture(texture);
}

void Text::Recalculate()
{
    font_size = std::min(original_font_size, Text::CalculateFontSize(size, label));

    bgRect.x = position.x;
    bgRect.y = position.y;
    bgRect.w = size.x;
    bgRect.h = size.y;
}

int Text::CalculateFontSize(const Vector2 &bg_size, std::string label)
{
    int minFs = 10;
    int maxFs = 100;
    int perfectFs = -1;

    SDL_Rect rect;
    rect.w = rect.h = 100000;

    while (minFs <= maxFs)
    {
        int midFs = (minFs + maxFs) / 2;
        TTF_SetFontSize(myFont, midFs);
        TTF_SizeText(myFont, label.c_str(), &rect.w, &rect.h);

        if (rect.w >= bg_size.x || rect.h >= bg_size.y)
        {
            maxFs = midFs - 1;
        }
        else
        {
            perfectFs = midFs;
            minFs = midFs + 1;
        }
    }

    return perfectFs;
}

void Text::SetLabel(std::string name, std::string label)
{
    if (UIs.find(name) != UIs.end())
    {
        UIs[name]->label = label;
        UI::Recalculate(UIs[name], UIs[name]->visible);
    }
}

//----------------------------------------

Slider::Slider(std::string name, const Vector2 &position, const Vector2 &size, float min_value, float max_value,
               float current_value, float step, std::function<void(float &value)> onValueChange, int font_size,
               int border_opacity)
    : UI(SLIDER, name, position, size, "", 0, font_size, border_opacity)
{
    print("creating", name, "slider");
    UIs[name] = this;
    this->min_value = min_value;
    this->max_value = max_value;
    this->previous_value = this->current_value = current_value;
    this->step = step;
    this->is_focus = this->mouse_hovering = false;
    this->onValueChange = onValueChange;
    print(name, "slider created");
}

void Slider::Update()
{
    TTF_SetFontSize(myFont, font_size);
    SDL_Surface *min_sf = TTF_RenderText_Blended(myFont, strRound(min_value, 2).c_str(), Color::white());
    SDL_Texture *min_texture = SDL_CreateTextureFromSurface(Game::renderer, min_sf);

    SDL_Surface *max_sf = TTF_RenderText_Blended(myFont, strRound(max_value, 2).c_str(), Color::white());
    SDL_Texture *max_texture = SDL_CreateTextureFromSurface(Game::renderer, max_sf);

    SDL_RenderCopy(Game::renderer, min_texture, NULL, &minRect);
    SDL_RenderCopy(Game::renderer, max_texture, NULL, &maxRect);

    Screen::SetDrawColor(Color::white());
    SDL_RenderFillRect(Game::renderer, &barRect);

    Screen::SetDrawColor(Color::white(border_opacity));
    SDL_RenderDrawRect(Game::renderer, &bgRect);

    btnRect.w = btnRect.h = barRect.h * 4.0f;
    btnRect.y = barRect.y + barRect.h / 2.0f - btnRect.h / 2.0f;

    mouse_hovering = InRange(EventHandler::MousePosition.x, min_position.x, max_position.x) &&
                     InRange(EventHandler::MousePosition.y, btnRect.y, btnRect.y + btnRect.h);

    if (is_focus)
    {
        btnRect.x =
            Clamp(EventHandler::MousePosition.x, min_position.x + btnRect.w / 2.0f, max_position.x - btnRect.w / 2.0f);
        current_value = Clamp((max_value - min_value) * (btnRect.x - (min_position.x + btnRect.w / 2.0f)) /
                                      (barRect.w - btnRect.w) +
                                  min_value,
                              min_value, max_value);
    }
    else
    {
        btnRect.x = Clamp((barRect.w - btnRect.w) * (current_value - min_value) / (max_value - min_value) +
                              min_position.x + btnRect.w / 2.0f,
                          min_position.x + btnRect.w / 2.0f, max_position.x - btnRect.w / 2.0f);
    }
    btnRect.x -= btnRect.w / 2.0f;

    DrawSprite("circ", Rect::GetPosition(btnRect), Rect::GetSize(btnRect), 1.0f, 0, 0);

    if (current_value != previous_value)
    {
        onValueChange(current_value);
        previous_value = current_value;
    }

    if (is_focus)
    {
        TTF_SetFontSize(myFont, original_font_size);
        SDL_Surface *current_sf = TTF_RenderText_Blended(myFont, strRound(current_value, 2).c_str(), Color::white());
        SDL_Texture *current_texture = SDL_CreateTextureFromSurface(Game::renderer, current_sf);
        SDL_Rect currentRect;
        TTF_SizeText(myFont, strRound(current_value, 2).c_str(), &currentRect.w, &currentRect.h);
        currentRect.y = btnRect.y - currentRect.h - 2.5f;
        currentRect.x = btnRect.x + btnRect.w / 2.0f - currentRect.w / 2.0f;
        Screen::SetDrawColor(Color::black());
        SDL_RenderFillRect(Game::renderer, &currentRect);
        SDL_RenderCopy(Game::renderer, current_texture, NULL, &currentRect);
        SDL_FreeSurface(current_sf);
        SDL_DestroyTexture(current_texture);
    }

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
        std::min(original_font_size, Text::CalculateFontSize(Vector2(minRect.w, minRect.h), strRound(min_value, 2)));

    TTF_SetFontSize(myFont, font_size);
    TTF_SizeText(myFont, strRound(min_value, 2).c_str(), &minRect.w, &minRect.h);

    minRect.x = minCenter.x - minRect.w / 2.0f;
    minRect.y = minCenter.y - minRect.h / 2.0f;

    font_size =
        std::min(original_font_size, Text::CalculateFontSize(Vector2(maxRect.w, maxRect.h), strRound(max_value, 2)));

    TTF_SetFontSize(myFont, font_size);
    TTF_SizeText(myFont, strRound(max_value, 2).c_str(), &maxRect.w, &maxRect.h);

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

Toggle::Toggle(std::string name, const Vector2 &position, const Vector2 &size, bool option,
               std::function<void(bool &option)> onSwitch, int border_opacity)
    : UI(TOGGLE, name, position, size, "", 0, 0, border_opacity)
{
    print("creating", name, "toggle");
    UIs[name] = this;
    this->option = option;
    this->mouse_click = this->mouse_hovering = false;
    this->onSwitch = onSwitch;
    print(name, "toggle created");
}

void Toggle::Update()
{
    mouse_hovering = InRange(EventHandler::MousePosition.x, switchRect.x, switchRect.x + switchRect.w) &&
                     InRange(EventHandler::MousePosition.y, switchRect.y, switchRect.y + switchRect.h);

    DrawSprite("toggle", Rect::GetPosition(switchRect), Rect::GetSize(switchRect), 1.0f, !option, !option);

    Screen::SetDrawColor(Color::white(border_opacity));
    SDL_RenderDrawRect(Game::renderer, &bgRect);
}

void Toggle::Recalculate()
{
    Vector2 center = Rect::GetCenter(position, size);
    switchRect.w = Screen::resolution.x / 18.0f;
    switchRect.h = (float)switchRect.w * 520.0f / 787.0f;
    switchRect.x = center.x - switchRect.w / 2.0f;
    switchRect.y = center.y - switchRect.h / 2.0f;

    bgRect.x = position.x;
    bgRect.y = position.y;
    bgRect.w = size.x;
    bgRect.h = size.y;
}

void Toggle::Switch(std::string name, bool option)
{
    if (UIs.find(name) != UIs.end())
        ((Toggle *)UIs[name])->option = option;
}

//----------------------------------------

Canvas::Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing, int margin,
               bool vertical, int border_opacity)
    : UI(CANVAS, name, position, size, "", 0, 0, border_opacity)
{
    print("creating", name, "canvas");
    UIs[name] = this;
    this->bg_opacity = bg_opacity;
    this->spacing = spacing;
    this->margin = margin;
    this->vertical = vertical;
    print(name, "canvas created");
}

void Canvas::AddComponent(UI *ui, int blocks)
{
    std::string new_name = name + "." + ui->name;
    UIs[new_name] = UIs[ui->name];
    UIs[ui->name] = nullptr;
    UIs.erase(ui->name);
    ui->name = new_name;

    Components.push_back(std::make_pair(ui->name, blocks));
    Recalculate();
}

void Canvas::AddComponents(const std::vector<std::pair<UI *, int>> &uis)
{
    for (auto &ui : uis)
    {
        std::string new_name = name + "." + ui.first->name;
        UIs[new_name] = UIs[ui.first->name];
        UIs[ui.first->name] = nullptr;
        UIs.erase(ui.first->name);
        ui.first->name = new_name;

        Components.push_back(std::make_pair(ui.first->name, ui.second));
    }
    Recalculate();
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
    Recalculate();
}

void Canvas::Recalculate()
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

        UI::Recalculate(ui, visible);
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
        UI::Recalculate(ui, visible);
    }
}

void Canvas::Update()
{
    SDL_Rect rect;
    rect.x = position.x;
    rect.y = position.y;
    rect.w = size.x;
    rect.h = size.y;
    Screen::SetDrawColor(Color::gray(16, bg_opacity));
    SDL_RenderFillRect(Game::renderer, &rect);
    Screen::SetDrawColor(Color::white(border_opacity));
    SDL_RenderDrawRect(Game::renderer, &rect);
}

SDL_Cursor *myCursor = nullptr;

bool ChangeCursor(std::string path)
{
    SDL_Surface *cssf = IMG_Load(path.c_str());
    if (!cssf)
        return 0;
    myCursor = SDL_CreateColorCursor(cssf, 0, 0);
    SDL_SetCursor(myCursor);
    SDL_FreeSurface(cssf);
    return 1;
}