#pragma once
#include "../../include/SDL2/SDL.h"
#include "../../include/SDL2/SDL_image.h"
#include "../game.h"
#include "msg.h"
#include "util.h"
#include "vector2.h"
#include <map>
#include <string>

class Sprite
{
  public:
    SDL_Texture *texture;
    std::string path;
    int max_frames;
    Vector2 real_size;
};

extern std::map<std::string, Sprite *> Sprites;

bool LoadSprite(std::string name, std::string path, int max_frames, Vector2 real_size);

void SetSpriteOpacity(std::string name, int opacity);

void DrawSprite(std::string name, const Vector2 &position, const Vector2 &size, float scale, int current_frame,
                bool flip = false);

void Animate(GameObject *gameobj, std::string sprite, bool flip = false);