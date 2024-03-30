#pragma once
#include "../../include/SDL2/SDL.h"
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
        std::string name, const Vector2 &position, const Vector2 &size, std::string label, int font_size,
        std::function<void()> onClick = []() {});
    Button(
        std::string name, std::string label, std::function<void()> onClick = []() {});

    void Update();
    static void DeleteButton(std::string name);
    static void DeleteButtons();
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
    static void DeleteCanvas(std::string name);
    static void DeleteCanvases();
};

extern std::map<std::string, Button *> Buttons;
extern std::map<std::string, Canvas *> Canvases;

int CalculateFontSize(const Vector2 &bg_size, std::string label);