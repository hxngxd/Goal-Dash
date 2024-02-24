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
    Move();

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

void Player::Move(){
    Vector2 startPos(position.x + player_size/6, position.y);
    Vector2 endPos(position.x + player_size/6*5, position.y + player_size);

    SDL_RenderDrawLine(renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(renderer, endPos.x, endPos.y, startPos.x, endPos.y);

    int tile_size = resolution.x/map_size;

    Vector2 startTile(
        (startPos.x + player_speed)/tile_size,
        (startPos.y + player_speed)/tile_size
    );

    Vector2 endTile(
        (endPos.x - player_speed)/tile_size,
        (endPos.y - player_speed)/tile_size
    );

    if (animation_state == run){
        if (animation_direction == left){
            velocity.l -= player_acceleration_rate;
            velocity.l = (velocity.l < -1 ? -1 : velocity.l);
            velocity.r = 0;
        }
        else{
            velocity.l = 0;
            velocity.r += player_acceleration_rate;
            velocity.r = (velocity.r > 1 ? 1 : velocity.r);
        }
    }
    else{
        velocity.l = velocity.r = 0;
    }

    position.x += (velocity.l + velocity.r) * player_speed;
    position.y += gravity;

    for (int i=std::max(0, (int)startTile.y);i<=std::min(map_size-1, (int)endTile.y);i++){
        if (startTile.x > 0 && tileMap[i][startTile.x-1]){
            if (position.x + player_size/6 - (int)startTile.x*tile_size <= 0){
                position.x -= velocity.l * player_speed;
            }
        }
    }

    for (int i=std::max(0, (int)startTile.y);i<=std::min(map_size-1, (int)endTile.y);i++){
        if (endTile.x < map_size - 1 && tileMap[i][endTile.x+1]){
            if (((int)endTile.x+1)*tile_size - position.x - player_size/6*5 <= 0){
                position.x -= velocity.r * player_speed;
            }
        }
    }

    for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
        if (startTile.y > 0 && tileMap[startTile.y-1][i]){
            if (position.y - (int)startTile.y*tile_size <= 0){
                position.y -= velocity.u * player_speed;
            }
        }
    }

    for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
        if (endTile.y < map_size - 1 && tileMap[endTile.y+1][i]){
            if (((int)endTile.y+1)*tile_size - position.y - player_size <= 0){
                position.y -= gravity;
            }
        }
    }

    std::cout << animation_state << " " << velocity.l << " " << velocity.r << std::endl;
}