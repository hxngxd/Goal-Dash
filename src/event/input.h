#pragma once
#include "../datalib/util.h"
#include "../gameobject/gameobject.h"

class EventHandler
{
  public:
    static Vector2 MousePosition;
    static bool isMouseLeft;
    static std::map<std::string, std::function<void()>> MouseDownActions;
    static std::map<std::string, std::function<void()>> MouseUpActions;
    static bool allow_ui;

    static void PlayerInputHandler(Player *player, Keys &keys);
    static void MouseInputHandler();
    static void TextInputHandler();
    static std::string *currentInputtingText;
    static int currentMaximumInputLength;

    static void Update();
};