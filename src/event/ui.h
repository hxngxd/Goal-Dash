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
    bool visible;

    std::string label;
    int original_font_size;
    int font_size;
    int label_alignment;
    SDL_Rect bgRect;

    bool mouse_hovering;

    UI(int type, std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
       int font_size, int border_opacity);

    static void Start();
    static void Update();

    static void RemoveUI(std::string name);
    static void RemoveUIs();
    static void RemovingUI(std::string name);
    static void RemovingUIs();

    static void SetVisible(std::string name, bool visible);
    static void Recalculate(UI *ui, bool visible);
    static void Recalculate(std::string name);
};

class Button : public UI
{
  public:
    bool enabled, selected;
    std::function<void()> onClick;
    bool hovering_sound, mouse_click;
    int lastButtonClick;

    SDL_Rect labelRect;

    Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
           std::function<void()> onClick, int font_size = 150, int border_opacity = 0);

    void Update();
    void Recalculate();
};

class Text : public UI
{
  public:
    SDL_Rect labelRect;
    bool caret;
    bool toggle_caret;
    int caret_opacity;

    Text(std::string name, const Vector2 &position, const Vector2 &size, std::string label, int label_alignment,
         int font_size = 150, int border_opacity = 0, bool caret = 0);

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
    float previous_value, current_value;
    float step;
    bool is_focus;

    std::function<void(float &value)> onValueChange;

    SDL_Rect barRect;
    SDL_Rect minRect;
    SDL_Rect maxRect;
    SDL_Rect btnRect;

    Slider(std::string name, const Vector2 &position, const Vector2 &size, float min_value, float max_value,
           float current_value, float step, std::function<void(float &value)> onValueChange, int font_size = 150,
           int border_opacity = 0);

    void Update();
    void Recalculate();

    static void SetValue(std::string name, float value);
};

class Toggle : public UI
{
  public:
    bool option;
    bool mouse_click;
    std::function<void(bool &option)> onSwitch;

    SDL_Rect switchRect;

    Toggle(std::string name, const Vector2 &position, const Vector2 &size, bool option,
           std::function<void(bool &option)> onSwitch, int border_opacity = 0);

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
           int margin = 0, bool vertical = true, int border_opacity = 0);

    std::vector<std::pair<std::string, int>> Components;

    void AddComponent(UI *ui, int blocks = 1);
    void AddComponents(const std::vector<std::pair<UI *, int>> &uis);

    void RemoveComponent(std::string name);
    void Recalculate();

    void Update();
};

extern SDL_Cursor *myCursor;
bool ChangeCursor(std::string path);
