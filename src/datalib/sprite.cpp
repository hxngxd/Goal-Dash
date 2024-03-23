#include "sprite.h"

std::map<std::string, Sprite *> Sprites;

bool LoadSprite(std::string name, std::string path, int maxFrames, Vector2 realSize)
{
    print("loading sprite:", path);
    Sprites[name] = new Sprite();
    SDL_Surface *sf = IMG_Load(path.c_str());
    Sprites[name]->texture = SDL_CreateTextureFromSurface(Game::renderer, sf);
    if (!Sprites[name]->texture)
    {
        delete Sprites[name];
        Sprites[name] = nullptr;
        Sprites.erase(name);
        print("failed to load sprite:", path);
        return 0;
    }
    print(path, "ok");
    Sprites[name]->path = path;
    Sprites[name]->maxFrames = maxFrames;
    Sprites[name]->realSize = realSize;
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
    SDL_RenderCopyEx(Game::renderer, sprite->texture, &src, &dst, 0, NULL,
                     (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}