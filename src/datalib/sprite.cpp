#include "sprite.h"

std::map<std::string, Sprite *> Sprites;

bool LoadSprite(std::string name, std::string path, int max_frames, Vector2 real_size)
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
    sp->max_frames = max_frames;
    sp->real_size = real_size;
    SDL_FreeSurface(sf);
    return 1;
}

void SetSpriteOpacity(std::string name, int opacity)
{
    if (Sprites[name]->texture)
        SDL_SetTextureAlphaMod(Sprites[name]->texture, opacity);
}

void DrawSprite(std::string name, const Vector2 &position, const Vector2 &size, float scale, int current_frame,
                bool flip)
{
    Sprite *&sprite = Sprites[name];
    SDL_Rect src;
    src.x = (current_frame % sprite->max_frames) * sprite->real_size.x;
    src.y = 0;
    src.w = sprite->real_size.x;
    src.h = sprite->real_size.y;
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
        gameobj->current_frame++;
        if (gameobj->current_frame >= Sprites[sprite]->max_frames)
            gameobj->current_frame = 0;
        gameobj->animation_delay = Ticks;
    }
    DrawSprite(sprite, gameobj->position, gameobj->size, gameobj->scale, gameobj->current_frame, flip);
}