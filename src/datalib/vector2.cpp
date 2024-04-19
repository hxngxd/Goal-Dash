#include "vector2.h"
#include "util.h"

Vector2 Vector2::Right = Vector2(1, 0);
Vector2 Vector2::Left = Vector2(-1, 0);
Vector2 Vector2::Down = Vector2(0, 1);
Vector2 Vector2::Up = Vector2(0, -1);

float Distance(const Vector2 &v1, const Vector2 &v2)
{
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    return sqrt(dx * dx + dy * dy);
}

std::ostream &operator<<(std::ostream &out, const Vector2 &v)
{
    out << "(" << v.x << ", " << v.y << ")";
    return out;
}

void operator+=(Vector2 &v1, const Vector2 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
}

void operator-=(Vector2 &v1, const Vector2 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
}

void operator*=(Vector2 &v, float k)
{
    v.x *= k;
    v.y *= k;
}

void operator/=(Vector2 &v, float k)
{
    v.x /= k;
    v.y /= k;
}

bool operator==(const Vector2 &v1, const Vector2 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}

bool operator!=(const Vector2 &v1, const Vector2 &v2)
{
    return (v1.x != v2.x) || (v1.y != v2.y);
}

bool operator<(const Vector2 &v1, const Vector2 &v2)
{
    return (v1.x < v2.x) || (!(v2.x < v1.x) && v1.y < v2.y);
}

Vector2 operator+(const Vector2 &v1, const Vector2 &v2)
{
    return Vector2(v1.x + v2.x, v1.y + v2.y);
}

Vector2 operator-(const Vector2 &v1, const Vector2 &v2)
{
    return Vector2(v1.x - v2.x, v1.y - v2.y);
}

Vector2 operator*(const Vector2 &v, float k)
{
    return Vector2(v.x * k, v.y * k);
}

Vector2 operator/(const Vector2 &v, float k)
{
    return Vector2(v.x / k, v.y / k);
}

Vector2 operator*(float k, const Vector2 &v)
{
    return Vector2(v.x * k, v.y * k);
}

Vector2 operator-(const Vector2 &v)
{
    return Vector2(-v.x, -v.y);
}

Vector2 Max(const Vector2 &v1, const Vector2 &v2)
{
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
}

Vector2 Min(const Vector2 &v1, const Vector2 &v2)
{
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
}

Vector2 Int(const Vector2 &v)
{
    return Vector2((int)v.x, (int)v.y);
}

void Swap(Vector2 &v)
{
    float tmp = v.x;
    v.x = v.y;
    v.y = tmp;
}

bool InRange(const Vector2 &v, const Vector2 &mn, const Vector2 &mx)
{
    return mn.x <= v.x && v.x <= mx.x && mn.y <= v.y && v.y <= mx.y;
}

std::size_t Vector2Hash::operator()(const Vector2 &v) const
{
    std::hash<float> hash_fn;
    return hash_fn(v.x) ^ hash_fn(v.y);
}

bool Vector2Equal::operator()(const Vector2 &lhs, const Vector2 &rhs) const
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool TransformVector2(Vector2 *v, Vector2 dest, float speed, float minimum_from_dest)
{
    if (!v)
        return 1;

    if (abs(v->x - dest.x) <= minimum_from_dest && abs(v->y - dest.y) <= minimum_from_dest)
    {
        v->x = dest.x;
        v->y = dest.y;
        return 1;
    }

    bool TransformX = TransformValue(&v->x, dest.x, speed * abs(v->x - dest.x));
    bool TransformY = TransformValue(&v->y, dest.y, speed * abs(v->y - dest.y));

    return TransformX && TransformY;
}