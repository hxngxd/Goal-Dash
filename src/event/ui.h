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
    float scale;
    int bg_opacity;
    int label_opacity;
    Uint32 DFid[3] = {0, 0, 0};

    static void Update();
};

class Button : public UI
{
  public:
    std::string label;
    int font_size;
    std::function<void()> onClick;

    static bool CreateButton(
        std::string name, const Vector2 &position, std::string label, std::function<void()> onClick = []() {});

    void Update();
    static void DeleteButtons();
};

class Text : public UI
{
  public:
    std::string label;
    int font_size;

    static bool CreateText(std::string name, const Vector2 &position, std::string label, int font_size);

    void Update();
    static void DeleteTexts();
};

extern std::map<std::string, Button *> Buttons;
extern std::string hoverButton, downButton, upButton;
extern int normalFS, hoverFS, clickFS, lastClicked;

extern std::map<std::string, Text *> Texts;
extern int createTime, stopTime, startTime;