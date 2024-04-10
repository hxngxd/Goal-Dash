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

    int type;
    std::string name;
    Vector2 position;
    Vector2 size;
    int bg_opacity;
    int border_opacity;

    std::string label;
    int original_font_size;
    int font_size;

    UI(int type, std::string name, const Vector2 &position, const Vector2 &size, std::string label, int font_size);

    static void Update();

    static void DeleteUI(std::string name);
    static void DeleteUIs();
};

class Button : public UI
{
  public:
    bool enabled;
    std::function<void()> onClick;
    bool hovering_sound, button_mouse_hovering, button_mouse_click;
    int lastButtonClick;

    Button(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
           std::function<void()> onClick, int font_size = 150);

    void Update();
};

class Text : public UI
{
  public:
    Text(std::string name, const Vector2 &position, Vector2 &size, std::string label, int font_size = 150);

    static int CalculateFontSize(const Vector2 &bg_size, std::string label);

    void Update();
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