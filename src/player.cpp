#include "game.h"
#include "render.h"
#include "keyboard.h"

void Player::Init(const char * name, Vector2 position){
    this->name = name;
    this->position = position;
}

Player::~Player(){
    
}

void Player::Update(){
    Collision();
    KeyboardHandler::PlayerMovement(*this);

    position.x += ((collide_left ? 0 : velocity.l) + (collide_right ? 0 : velocity.r)) * player_speed;
    position.y += ((collide_up ? 0 : velocity.u) + (collide_down ? 0 : velocity.d)) * player_speed;
    ScreenLimit();

    Sprite * state = nullptr;
    switch (animation_state){
        case run:
            state = &sprite_run;
            break;
        default:
            state = &sprite_idle;
            break;
        
    }
    state_frames = state->frames;
    Renderer::DrawSprite(*state, position, Vector2(player_size), std::min(frame, state_frames), animation_direction == left);
}

void Player::Animation(){
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000/animation_speed){
        frame += 1;
        if (frame >= state_frames) frame = 0;
        animation_delay = currentTicks;
    }
}

void Player::ScreenLimit(){
    position = Vector2::max(position, Vector2(-player_size/6, 0));
    position = Vector2::min(position, resolution - Vector2(player_size/6*5, player_size));
}

void Player::Collision(){
    Vector2 startPos(position.x + player_size/6, position.y);
    Vector2 endPos(position.x + player_size/6*5, position.y + player_size);
    
    // SDL_RenderDrawLine(renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    // SDL_RenderDrawLine(renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    // SDL_RenderDrawLine(renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    // SDL_RenderDrawLine(renderer, endPos.x, endPos.y, startPos.x, endPos.y);

    int tile_size = resolution.x/map_size;

    Vector2 startTile(
        (startPos.x + player_speed + velocity.l)/tile_size,
        (startPos.y + player_speed + velocity.u)/tile_size
    );

    Vector2 endTile(
        (endPos.x - player_speed + velocity.r)/tile_size,
        (endPos.y - player_speed + velocity.d)/tile_size
    );

    Vector2 center = position + Vector2(player_size)/2;
    Vector2 centerTile = center/tile_size;

    bool collide = false;
    for (int i=std::max(0, (int)startTile.y);i<=std::min(map_size-1, (int)endTile.y);i++){
        if (tileMap[i][startTile.x-1]){
            int dx = (position.x + player_size/6) - (int)startTile.x*tile_size;
            if (dx <= 0) collide = collide || tileMap[i][startTile.x-1];
            break;
        }
    }
    collide_left = collide;
    // if (collide_left) std::cout << "theres a wall on your left" << std::endl;

    collide = false;
    for (int i=std::max(0, (int)startTile.y);i<=std::min(map_size-1, (int)endTile.y);i++){
        if (tileMap[i][endTile.x+1]){
            int dx = ((int)endTile.x+1)*tile_size - (position.x + player_size/6*5);
            if (dx <= 0) collide = collide || tileMap[i][endTile.x+1];
            break;
        }
    }
    collide_right = collide;
    // if (collide_right) std::cout << "theres a wall on your right" << std::endl;

    collide = false;
    for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
        if (tileMap[startTile.y-1][i]){
            int dy = position.y - (int)startTile.y*tile_size;
            if (dy <= 0) collide = collide || tileMap[startTile.y-1][i];
            break;
        }
    }
    collide_up = collide;
    // if (collide_up) std::cout << "theres a wall above you" << std::endl;

    collide = false;
    for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
        if (tileMap[endTile.y+1][i]){
            int dy = ((int)endTile.y+1)*tile_size - (position.y + player_size);
            if (dy <= 0) collide = collide || tileMap[endTile.y+1][i];
            break;
        }
    }
    collide_down = collide;
    // if (collide_down) std::cout << "theres a wall under you" << std::endl;
}