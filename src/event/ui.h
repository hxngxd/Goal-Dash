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
    std::string name;
    Vector2 position;
    Vector2 size;
    int bg_opacity;
    int label_opacity;
    std::string label;
    int font_size;
    int type;

    UI(int type, std::string name, int bg_opacity = 255);
    UI(int type, std::string name, std::string label, int bg_opacity = 255, int label_opacity = 255);

    static void Start();
    static void Update();

    static void DeleteUI(std::string name);
    static void DeleteUIs();
};

class Button : public UI
{
  public:
    std::function<void()> onClick;
    bool hovering_sound, button_mouse_hovering, button_mouse_click;
    int lastButtonClick;

    Button(
        std::string name, std::string label, std::function<void()> onClick = []() {});

    void Update();
};

class Text : public UI
{
  public:
    Text(std::string name, std::string label, int bg_opacity = 255, int label_opacity = 255);

    void Update();
};

class Canvas : public UI
{
  public:
    int spacing;
    int margin;
    bool vertical_alignment;

    Canvas(std::string name, const Vector2 &position, const Vector2 &size, int bg_opacity, int spacing, int margin,
           bool vertical_alignment = true);

    std::vector<std::string> Components;
    void AddComponents(std::string name);
    void AddComponents(const std::vector<std::string> &names);
    void RemoveComponents(std::string name);
    void RecalculateComponentsPosition();

    void Update();
};

extern std::map<std::string, UI *> UIs;

int CalculateFontSize(const Vector2 &bg_size, std::string label);