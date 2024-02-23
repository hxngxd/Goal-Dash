#pragma once
#include <bits/stdc++.h>

struct Vector2{
    float x;
    float y;
    
    Vector2() : x(0.0), y(0.0) {}
    Vector2(float v) : x(v), y(v) {} 
    Vector2(float x, float y) : x(x), y(y) {}

    friend std::ostream & operator << (std::ostream & out, Vector2 & v){
        out << "(" << v.x << ", " << v.y << ")";
    }

    void operator += (Vector2 v){
        x += v.x;
        y += v.y;
    }

    void operator -= (Vector2 v){
        x -= v.x;
        y -= v.y;
    }

    void operator *= (float k){
        x *= k;
        y *= k;
    }

    void operator /= (float k){
        x /= k;
        y /= k;
    }

    static Vector2 max(Vector2 v1, Vector2 v2){
        return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
    }

    static Vector2 min(Vector2 v1, Vector2 v2){
        return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
    }

    friend Vector2 operator + (Vector2 v1, Vector2 v2){
        return Vector2(v1.x + v2.x, v1.y + v2.y);
    }

    friend Vector2 operator - (Vector2 v1, Vector2 v2){
        return Vector2(v1.x - v2.x, v1.y - v2.y);
    }

    friend Vector2 operator * (Vector2 v, float k){
        return Vector2(v.x * k, v.y * k);
    }

    friend Vector2 operator / (Vector2 v, float k){
        return Vector2(v.x / k, v.y / k);
    }
};