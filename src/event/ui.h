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
    float scale;
    int bg_opacity;
    int label_opacity;
    std::string label;
    int font_size;

    static void Start();
    static void Update();
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
    Text(std::string name, int bg_opacity, std::string label);

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
    std::vector<std::pair<std::string, std::string>> Components;
    void AddComponents(std::string type, std::string name);
    void RemoveComponents(std::string type, std::string name);
    void RecalculateComponentsPosition();

    void Update();
};

extern std::map<std::string, Button *> Buttons;
extern std::map<std::string, Text *> Texts;
extern std::map<std::string, Canvas *> Canvases;

int CalculateFontSize(const Vector2 &bg_size, std::string label);

template <typename T> void DeleteUI(std::string name, std::map<std::string, T> &UIs, std::string type)
{
    if (UIs.find(name) == UIs.end())
        return;
    T &ui = UIs[name];
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
    UIs.erase(name);
    print(type, name, "deleted");
}

template <typename T> void DeleteUIs(std::map<std::string, T> &UIs, std::string type)
{
    print("deleting", type);
    for (auto &ui : UIs)
        DeleteUI<T>(ui.first, UIs, type);
    print(type, "deleted");
    UIs.clear();
}