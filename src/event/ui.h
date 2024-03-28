#pragma once
#include "../../include/SDL2/SDL.h"
#include "../datalib/vector2.h"
#include <functional>
#include <map>
#include <string>

class UI
{
  public:
    std::string name;
    Vector2 position;
    Vector2 size;
    float scale;
    int bg_opacity;
    int label_opacity;

    static void Start();
    static void Update();
};

class Button : public UI
{
  public:
    std::string label;
    int font_size;
    std::function<void()> onClick;
    bool button_mouse_hovering, button_mouse_click;
    int lastButtonClick;

    static bool CreateButton(
        std::string name, const Vector2 &position, const Vector2 &size, std::string label, int font_size,
        std::function<void()> onClick = []() {});

    void Update();
    static void DeleteButtons();
};

class Text : public UI
{
  public:
    std::string label;
    int font_size;

    static bool CreateText(std::string name, const Vector2 &position, const Vector2 &size, std::string label,
                           int font_size);

    void Update();
    static void DeleteTexts();
};

extern std::map<std::string, Button *> Buttons;
extern std::map<std::string, Text *> Texts;