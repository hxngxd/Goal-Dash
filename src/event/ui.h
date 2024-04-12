#pragma once
#include "../../include/SDL2/SDL.h"
#include "../datalib/msg.h"
#include "../datalib/util.h"
#include "../datalib/vector2.h"
#include <functional>
#include <map>
#include <string>
#include <unordered_set>

class UI
{
  public:
    static std::map<std::string, UI *> UIs;
    static std::vector<std::string> WaitingForRemoval;

    int type;
    std::string name;
    Vector2 position;
    Vector2 size;
    int bg_opacity;
    int border_opacity;

    std::string label;
    int original_font_size;
    int font_size;
    int label_alignment;
    SDL_Rect bgRect;

    bool mouse_hovering;

    UI(int type, std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
       int font_size);

    static void Start();
    static void Update();

    static void RemoveUI(std::string name);
    static void RemoveUIs();
    static void RemovingUI(std::string name);
    static void RemovingUIs();
};

class Button : public UI
{
  public:
    bool enabled;
    std::function<void()> onClick;
    bool hovering_sound, mouse_click;
    int lastButtonClick;

    SDL_Rect labelRect;

    Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
           std::function<void()> onClick, int font_size = 150);

    void Update();
    void Recalculate();
};

class Text : public UI
{
  public:
    SDL_Rect labelRect;

    Text(std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
         int font_size = 150);

    static int CalculateFontSize(const Vector2 &bg_size, std::string label);

    void Update();
    void Recalculate();

    static void SetLabel(std::string name, std::string label);
};

class Slider : public UI
{
  public:
    Vector2 min_position;
    Vector2 max_position;
    float min_value;
    float max_value;
    float current_value;
    float step;
    bool is_focus;

    SDL_Rect barRect;
    SDL_Rect minRect;
    SDL_Rect maxRect;
    SDL_Rect btnRect;

    Slider(std::string name, const Vector2 &position, const Vector2 &size, float min_value, float max_value,
           float current_value, float step, int font_size = 150);

    void Update();
    void Recalculate();

    static void SetValue(std::string name, float value);
};

class Toggle : public UI
{
  public:
    bool option;
    bool mouse_click;

    SDL_Rect switchRect;

    Toggle(std::string name, const Vector2 &position, const Vector2 &size, bool option);

    void Update();
    void Recalculate();

    static void Switch(std::string name, bool option);
};

class Canvas : public UI
{
  public:
    int spacing;
    int margin;
    bool vertical;

    Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing = 0,
           int margin = 0, bool vertical = true);

    std::vector<std::pair<std::string, int>> Components;

    void AddComponent(std::string name, int blocks = 1);
    void AddComponents(const std::vector<std::pair<std::string, int>> &names);

    void RemoveComponent(std::string name);
    void CalculateComponentsPosition();

    void Update();
};