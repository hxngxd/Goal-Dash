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
    KeyboardHandler::PlayerMovement(*this);

    position.x += (velocity.l + velocity.r) * player_speed;
    position.y += (velocity.u + velocity.d) * player_speed;
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