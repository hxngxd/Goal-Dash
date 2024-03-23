#pragma once
#include "../datalib/vector2.h"
#include <functional>
#include <map>
#include <string>

class UI
{
  public:
    std::string name;
    Vector2 position;
    float scale;
    float bg_opacity;
    Uint32 DFid[3] = {0, 0, 0};

    static void Update();
    static void DeleteUIs();
};

class Button : public UI
{
  public:
    std::string label;
    float font_size;
    std::function<void()> onClick;

    static bool CreateButton(
        std::string name, const Vector2 &position, std::string label, std::function<void()> onClick = []() {});

    void Update();
};

extern std::map<std::string, Button *> Buttons;
extern std::string hoverButton, downButton, upButton;