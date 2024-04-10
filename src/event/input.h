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

    static void PlayerInputHandler(Player *player, Keys &keys);
    static void MouseInputHandler();
    static void Update();
};