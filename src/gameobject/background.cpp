#include "../datalib/PerlinNoise/PerlinNoise.hpp"
#include "../datalib/sprite.h"
#include "../event/input.h"
#include "../game.h"
#include "gameobject.h"

std::vector<Background *> Background::Backgrounds;

Background::Background(std::string name, float scale)
{
    this->opacity = 64;
    this->toggle = true;
    this->name = name;
    this->position = Vector2(0);
    this->size = Screen::resolution;
    this->max_frames = Sprites[name]->max_frames;
    this->scale = scale;
}

bool Background::loadBackground(std::string path, Vector2 realSize)
{
    std::string name = "bg-" + str(Backgrounds.size());
    float scale = Backgrounds.size() * 0.05f + 1.05f;
    if (Sprites.find(name) == Sprites.end())
    {
        if (!LoadSprite(name, path, 1, realSize))
            return 0;
    }
    print("creating background", name);
    Backgrounds.push_back(new Background(name, scale));
    print("background", name, "created");
    SDL_SetTextureBlendMode(Sprites[name]->texture, SDL_BLENDMODE_BLEND);
    return 1;
}

void Background::Update()
{
    if (Backgrounds[1]->toggle)
    {
        Backgrounds[1]->opacity += 2;
        if (Backgrounds[1]->opacity >= 248)
            Backgrounds[1]->toggle = false;
    }
    else
    {
        Backgrounds[1]->opacity -= 2;
        if (Backgrounds[1]->opacity <= 8)
            Backgrounds[1]->toggle = true;
    }

    SetSpriteOpacity(Backgrounds[1]->name, Backgrounds[1]->opacity);

    Backgrounds[2]->opacity = 248 - Backgrounds[1]->opacity;
    SetSpriteOpacity(Backgrounds[2]->name, Backgrounds[2]->opacity);
}

void Background::MoveRelativeTo(const Vector2 &position)
{
    for (auto &bg : Backgrounds)
    {
        TransformVector2(&bg->position, -position * (bg->scale - 1.0f) * 0.5f, 0.05, 0);
        DrawSprite(bg->name, bg->position, bg->size, bg->scale, bg->max_frames, 0);
    }
}