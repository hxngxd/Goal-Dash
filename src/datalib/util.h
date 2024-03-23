#pragma once
#include "../../include/SDL2/SDL.h"
#include "../../include/SDL2/SDL_ttf.h"
#include "../func/func.h"
#include "chrono"
#include "random"
#include "vector2.h"

//----------------------------------------

extern TTF_Font *myFont;

//----------------------------------------

void TransformValue(float *value, float dest, float speed, Uint32 delay_time);

//----------------------------------------

template <typename T> T Clamp(T value, T min_value, T max_value)
{
    if (value < min_value)
        return min_value;
    if (value > max_value)
        return max_value;
    return value;
}

//----------------------------------------

template <typename T> bool InRange(T value, T min_value, T max_value)
{
    return min_value <= value && value <= max_value;
}

//----------------------------------------

template <typename T> class RandomGenerator
{
  private:
    std::mt19937 gen;
    std::uniform_int_distribution<> distrib;

  public:
    RandomGenerator()
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        gen = std::mt19937(seed);
        distrib = std::uniform_int_distribution<>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    }
    T rand()
    {
        return distrib(gen);
    }
};

extern RandomGenerator<Uint32> RandUint32;

//----------------------------------------

struct Keys
{
    SDL_KeyCode right, left, down, up, space;
    Keys() = default;
    Keys(SDL_KeyCode right, SDL_KeyCode left, SDL_KeyCode down, SDL_KeyCode up, SDL_KeyCode space)
        : right(right), left(left), down(down), up(up), space(space)
    {
    }
};

extern Keys left_keys, right_keys;

//----------------------------------------

struct Color
{
    static SDL_Color white(Uint8 a)
    {
        return {255, 255, 255, a};
    }
    static SDL_Color black(Uint8 a)
    {
        return {0, 0, 0, a};
    }
    static SDL_Color blue(Uint8 a)
    {
        return {0, 0, 255, a};
    }
    static SDL_Color red(Uint8 a)
    {
        return {255, 0, 0, a};
    }
    static SDL_Color green(Uint8 a)
    {
        return {0, 255, 0, a};
    }
    static SDL_Color violet(Uint8 a)
    {
        return {238, 130, 238, a};
    }
    static SDL_Color pink(Uint8 a)
    {
        return {255, 192, 203, a};
    }
    static SDL_Color yellow(Uint8 a)
    {
        return {255, 255, 0, a};
    }
    static SDL_Color orange(Uint8 a)
    {
        return {255, 165, 0, a};
    }
    static SDL_Color cyan(Uint8 a)
    {
        return {0, 255, 255, a};
    }
    static SDL_Color transparent;
};

//----------------------------------------

struct Rect
{
    static bool IsColliding(const Vector2 &first_position, const Vector2 &first_size, const Vector2 &second_position,
                            const Vector2 &second_size, float eps);

    static Vector2 GetCenter(const Vector2 &position, const Vector2 &size);

    static SDL_Rect Rescale(const Vector2 &position, const Vector2 &size, float scale);

    static SDL_Rect Rescale(SDL_Rect rect, float scale);
};

//----------------------------------------

struct Direction
{
    float l, r, u, d;
    Direction() : l(0), r(0), u(0), d(0)
    {
    }
    Direction(float l, float r, float u, float d) : l(l), r(r), u(u), d(d)
    {
    }
};

//----------------------------------------

enum map_types
{
    EMPTY = 0,
    SPAWN = 1,
    WIN = 2,
    WALL = 4,
    COIN = 8,
    DAMAGE = 16
};

enum animation_states
{
    IDLE,
    RUN,
    JUMP
};

enum animation_directions
{
    LEFT,
    RIGHT
};

//----------------------------------------

struct PropertiesType
{
    int i;
    std::string s;
    float f;
    bool b;
};

//----------------------------------------