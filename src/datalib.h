#pragma once
#include <bits/stdc++.h>
#include "../include/SDL2/SDL.h"
#include "../include/SDL2/SDL_image.h"
#include "../include/SDL2/SDL_mixer.h"

struct Vector2{
    float x, y;
    Vector2() : x(0.0), y(0.0) {}
    Vector2(float value) : x(value), y(value) {} 
    Vector2(float x, float y) : x(x), y(y) {}
    float distance(const Vector2 & other);
};

std::ostream & operator << (std::ostream & out, const Vector2 & v);
void operator += (Vector2 & v1, const Vector2 & v2);
void operator -= (Vector2 & v1, const Vector2 & v2);
void operator *= (Vector2 & v, float k);
void operator /= (Vector2 & v, float k);
Vector2 operator + (const Vector2 & v1, const Vector2 & v2);
Vector2 operator - (const Vector2 & v1, const Vector2 & v2);
Vector2 operator * (const Vector2 & v, float k);
Vector2 operator / (const Vector2 & v, float k);
Vector2 operator - (const Vector2 & v);

Vector2 max(const Vector2 & v1, const Vector2 & v2);
Vector2 min(const Vector2 & v1, const Vector2 & v2);

Vector2 IntV2(const Vector2 & v);
bool IsV2InRange(Vector2 & v, const Vector2 & mn, const Vector2 & mx);

extern Vector2 v2right, v2left, v2down, v2up;

struct Vector2Hash {
    std::size_t operator()(const Vector2 & v) const;
};

struct Vector2Equal {
    bool operator()(const Vector2 & lhs, const Vector2 & rhs) const;
};

struct Direction{
    float l, r, u, d;
    Direction () : l(0), r(0), u(0), d(0) {}
    Direction (float l, float r, float u, float d) : l(l), r(r), u(u), d(d) {}
};

struct Color{
    static SDL_Color white  (Uint8 a) { return {255, 255, 255, a}; }
    static SDL_Color black  (Uint8 a) { return {  0,   0,   0, a}; }
    static SDL_Color blue   (Uint8 a) { return {  0,   0, 255, a}; }
    static SDL_Color red    (Uint8 a) { return {255,   0,   0, a}; }
    static SDL_Color green  (Uint8 a) { return {  0, 255,   0, a}; }
    static SDL_Color violet (Uint8 a) { return {238, 130, 238, a}; }
    static SDL_Color pink   (Uint8 a) { return {255, 192, 203, a}; }
    static SDL_Color yellow (Uint8 a) { return {255, 255,   0, a}; }
    static SDL_Color orange (Uint8 a) { return {255, 165,   0, a}; }
    static SDL_Color cyan   (Uint8 a) { return {  0, 255, 255, a}; }
};

struct Rect{
    static SDL_Rect Square (int res) { return {0, 0, res, res}; }

    static bool isCollide(const Vector2 & first_position, const Vector2 & first_size, const Vector2 & second_position, const Vector2 & second_size);

    static Vector2 getCenter(const Vector2 & position, const Vector2 & size);
};

enum map_types{
    EMPTY = 0,
    SPAWN = 1,
    WIN = 2,
    WALL = 4,
    COIN = 8,
    DAMAGE = 16
};

enum animation_states{
    IDLE, RUN, JUMP
};

enum animation_directions{
    LEFT, RIGHT
};

enum msg_types{
    normal,
    success,
    fail,
    error,
    logging
};

class Sprite{
public:
    SDL_Texture * texture;
    std::string path;
    int maxFrames;
    Vector2 realSize;
};

extern std::map<std::string, Sprite*> Sprites;

bool loadSprite(std::string name, std::string path, int maxFrames, Vector2 realSize);

extern std::map<std::string, Mix_Chunk*> Sounds;
extern std::map<std::string, Mix_Music*> Musics;

bool loadSoundEffect(std::string name, std::string path);
void playSound(std::string name, int channel, int loop);
void stopSound(int channel);

bool loadMusic(std::string name, std::string path);
void playMusic(std::string name, int loop);
void stopMusic();

enum mixer_channels{
    coin_channel = 0,
    jump_channel,
    run_channel,
    fall_channel
};

float clamp(float value, float mn, float mx);