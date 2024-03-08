#include "datalib.h"
#include "game.h"

std::map<std::string, Sprite*> Sprites;


Vector2 v2right = Vector2(1, 0);
Vector2 v2left = Vector2(-1, 0);
Vector2 v2down = Vector2(0, 1);
Vector2 v2up = Vector2(0, -1);

Vector2 IntV2(const Vector2 & v){
    return Vector2((int)v.x, (int)v.y);
}

std::ostream & operator << (std::ostream & out, const Vector2 & v){
    out << "(" << v.x << ", " << v.y << ")";
}

void operator += (Vector2 & v1, const Vector2 & v2){
    v1.x += v2.x;
    v1.y += v2.y;
}

void operator -= (Vector2 & v1, const Vector2 & v2){
    v1.x -= v2.x;
    v1.y -= v2.y;
}

void operator *= (Vector2 & v, float k){
    v.x *= k;
    v.y *= k;
}

void operator /= (Vector2 & v, float k){
    v.x /= k;
    v.y /= k;
}

Vector2 operator + (const Vector2 & v1, const Vector2 & v2){
    return Vector2(v1.x + v2.x, v1.y + v2.y);
}

Vector2 operator - (const Vector2 & v1, const Vector2 & v2){
    return Vector2(v1.x - v2.x, v1.y - v2.y);
}

Vector2 operator * (const Vector2 & v, float k){
    return Vector2 (v.x * k, v.y * k);
}

Vector2 operator / (const Vector2 & v, float k){
    return Vector2 (v.x / k, v.y / k);
}

Vector2 operator - (const Vector2 & v){
    return Vector2(-v.x, -v.y);
}

Vector2 max(const Vector2 & v1, const Vector2 & v2){
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
}
Vector2 min(const Vector2 & v1, const Vector2 & v2){
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
}

float Vector2::distance(const Vector2 & other){
    float dx = other.x - x;
    float dy = other.y - y;
    return sqrt(dx*dx + dy*dy);
}

bool IsV2InRange(Vector2 & v, const Vector2 & mn, const Vector2 & mx){
    return mn.x <= v.x && v.x <= mx.x && mn.y <= v.y && v.y <= mx.y;
}

std::size_t Vector2Hash::operator()(const Vector2 & v) const {
    std::hash<float> hash_fn;
    return hash_fn(v.x) ^ hash_fn(v.y);
}

bool Vector2Equal::operator()(const Vector2 & lhs, const Vector2 & rhs) const {
    return lhs.x == rhs.x && lhs.y == rhs.y;;
}

bool Rect::isCollide(const Vector2 & first_position, const Vector2 & first_size, const Vector2 & second_position, const Vector2 & second_size){
    float c_x = abs(first_position.x - second_position.x) - first_size.x/2 - second_size.x/2;
    float c_y = abs(first_position.y - second_position.y) - first_size.y/2 - second_size.y/2;
    return c_x <= 0 && c_y <= 0;
}

Vector2 Rect::getCenter(const Vector2 & position, const Vector2 & size){
    return position + size/2;
}

bool loadSprite(std::string name, std::string path, int maxFrames, Vector2 realSize){
    ShowMsg(2, normal, "trying to load " + path + "...");
    Sprites[name] = new Sprite();
    SDL_Surface * sf = IMG_Load(path.c_str());
    Sprites[name]->texture = SDL_CreateTextureFromSurface(Game::renderer, sf);

    if (Sprites[name]->texture==nullptr){
        delete Sprites[name];
        Sprites[name] = nullptr;
        Sprites.erase(name);
        ShowMsg(3, fail, "failed.");
        return 0;
    }
    
    ShowMsg(3, success, "done.");
    Sprites[name]->path = path;
    Sprites[name]->maxFrames = maxFrames;
    Sprites[name]->realSize = realSize;

    SDL_FreeSurface(sf);
    return 1;
}

float clamp(float value, float mn, float mx){
    if (value < mn) return mn;
    if (value > mx) return mx;
    return value;
}