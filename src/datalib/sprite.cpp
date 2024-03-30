#include "sprite.h"

std::map<std::string, Sprite *> Sprites;

bool LoadSprite(std::string name, std::string path, int maxFrames, Vector2 realSize)
{
    print("loading sprite:", path);
    Sprites[name] = new Sprite();
    Sprite *&sp = Sprites[name];

    SDL_Surface *sf = IMG_Load(path.c_str());
    sp->texture = SDL_CreateTextureFromSurface(Game::renderer, sf);
    if (!sp->texture)
    {
        delete sp;
        sp = nullptr;
        Sprites.erase(name);
        print("failed to load sprite:", path);
        return 0;
    }
    print(path, "ok");
    sp->path = path;
    sp->maxFrames = maxFrames;
    sp->realSize = realSize;
    SDL_FreeSurface(sf);
    return 1;
}

void SetSpriteOpacity(std::string name, int opacity)
{
    if (Sprites[name]->texture)
        SDL_SetTextureAlphaMod(Sprites[name]->texture, opacity);
}

void DrawSprite(std::string name, const Vector2 &position, const Vector2 &size, float scale, int currentFrame,
                bool flip)
{
    Sprite *&sprite = Sprites[name];
    SDL_Rect src;
    src.x = (currentFrame % sprite->maxFrames) * sprite->realSize.x;
    src.y = 0;
    src.w = sprite->realSize.x;
    src.h = sprite->realSize.y;
    SDL_Rect dst = Rect::Rescale(position, size, scale);
    SDL_SetRenderTarget(Game::renderer, sprite->texture);
    SDL_RenderCopyEx(Game::renderer, sprite->texture, &src, &dst, 0, NULL,
                     (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}

void Animate(GameObject *gameobj, std::string sprite, bool flip)
{
    float Ticks = SDL_GetTicks();
    if (Ticks > gameobj->animation_delay + 1000.0f / gameobj->animation_speed)
    {
        gameobj->currentFrame++;
        if (gameobj->currentFrame >= Sprites[sprite]->maxFrames)
            gameobj->currentFrame = 0;
        gameobj->animation_delay = Ticks;
    }
    DrawSprite(sprite, gameobj->position, gameobj->size, gameobj->scale, gameobj->currentFrame, flip);
}