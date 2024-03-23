#include "util.h"

//----------------------------------------

TTF_Font *myFont = nullptr;

//----------------------------------------

RandomGenerator<Uint32> RandUint32;

//----------------------------------------

Keys left_keys(SDLK_d, SDLK_a, SDLK_s, SDLK_w, SDLK_SPACE);
Keys right_keys(SDLK_RIGHT, SDLK_LEFT, SDLK_DOWN, SDLK_UP, SDLK_KP_ENTER);

//----------------------------------------

SDL_Color Color::transparent = {0, 0, 0, 0};

//----------------------------------------

bool Rect::IsColliding(const Vector2 &first_position, const Vector2 &first_size, const Vector2 &second_position,
                       const Vector2 &second_size, float eps)
{
    float c_x = abs(first_position.x - second_position.x) - first_size.x / 2 - second_size.x / 2;
    float c_y = abs(first_position.y - second_position.y) - first_size.y / 2 - second_size.y / 2;
    return c_x <= eps && c_y <= eps;
}

Vector2 Rect::GetCenter(const Vector2 &position, const Vector2 &size)
{
    return position + size / 2;
}

SDL_Rect Rect::Rescale(const Vector2 &position, const Vector2 &size, float scale)
{
    SDL_Rect newRect;
    newRect.x = position.x + (1 - scale) * size.x * 0.5;
    newRect.y = position.y + (1 - scale) * size.y * 0.5;
    newRect.w = scale * size.x;
    newRect.h = scale * size.y;
    return newRect;
}

SDL_Rect Rect::Rescale(SDL_Rect rect, float scale)
{
    return Rescale(Vector2(rect.x, rect.y), Vector2(rect.w, rect.h), scale);
}