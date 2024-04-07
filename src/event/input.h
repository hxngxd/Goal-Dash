#pragma once
#include "../datalib/util.h"
#include "../gameobject/gameobject.h"

extern Vector2 mousePosition;
extern bool mouseLeft;

class EventHandler
{
  public:
    static void PlayerInputHandler(Player *player, Keys &keys);
    static void MouseInputHandler();
    static void Update();
};