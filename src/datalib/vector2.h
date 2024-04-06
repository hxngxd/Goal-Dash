#pragma once
#include <iostream>
#include <math.h>

struct Vector2
{
    float x, y;
    Vector2() : x(0.0), y(0.0)
    {
    }
    Vector2(float value) : x(value), y(value)
    {
    }
    Vector2(float x, float y) : x(x), y(y)
    {
    }
    static Vector2 Right, Left, Down, Up;
};

float Distance(const Vector2 &v1, const Vector2 &v2);

std::ostream &operator<<(std::ostream &out, const Vector2 &v);

void operator+=(Vector2 &v1, const Vector2 &v2);

void operator-=(Vector2 &v1, const Vector2 &v2);

void operator*=(Vector2 &v, float k);

void operator/=(Vector2 &v, float k);

bool operator==(const Vector2 &v1, const Vector2 &v2);

bool operator!=(const Vector2 &v1, const Vector2 &v2);

bool operator<(const Vector2 &v1, const Vector2 &v2);

Vector2 operator+(const Vector2 &v1, const Vector2 &v2);

Vector2 operator-(const Vector2 &v1, const Vector2 &v2);

Vector2 operator*(const Vector2 &v, float k);

Vector2 operator/(const Vector2 &v, float k);

Vector2 operator*(float k, const Vector2 &v);

Vector2 operator-(const Vector2 &v);

Vector2 Max(const Vector2 &v1, const Vector2 &v2);

Vector2 Min(const Vector2 &v1, const Vector2 &v2);

Vector2 Int(const Vector2 &v);

bool InRange(const Vector2 &v, const Vector2 &mn, const Vector2 &mx);

struct Vector2Hash
{
    std::size_t operator()(const Vector2 &v) const;
};

struct Vector2Equal
{
    bool operator()(const Vector2 &lhs, const Vector2 &rhs) const;
};

bool TransformVector2(Vector2 *v, Vector2 dest, float speed, float minimum_from_dest);