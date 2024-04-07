#include "util.h"
#include "msg.h"

//----------------------------------------

TTF_Font *myFont = nullptr;

//----------------------------------------

std::string FormatMS(int ms)
{
    float fsecond = (float)ms / 1000.0f;
    int second = fsecond;
    fsecond -= (float)second;
    fsecond *= 1000;
    int hour = second / 3600;
    second -= hour * 3600;
    int minute = second / 60;
    second -= minute * 60;
    auto addzero = [](int n, int length) {
        std::string strn = std::to_string(n);
        return std::string(length - strn.size(), '0') + strn;
    };
    return (hour ? std::to_string(hour) : "00") + ":" + addzero(minute, 2) + ":" + addzero(second, 2) + "." +
           addzero((int)fsecond, 3);
}

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
    newRect.x = position.x + ceil((1.0f - scale) * size.x * 0.5);
    newRect.y = position.y + ceil((1.0f - scale) * size.y * 0.5);
    newRect.w = ceil(scale * size.x);
    newRect.h = ceil(scale * size.y);
    return newRect;
}

SDL_Rect Rect::Rescale(SDL_Rect rect, float scale)
{
    return Rescale(Vector2(rect.x, rect.y), Vector2(rect.w, rect.h), scale);
}

Vector2 Rect::GetPosition(SDL_Rect rect)
{
    return Vector2(rect.x, rect.y);
}

Vector2 Rect::GetSize(SDL_Rect rect)
{
    return Vector2(rect.w, rect.h);
}