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

    position.x += (velocity.l + velocity.r) * speed;
    position.y += (velocity.u + velocity.d) * speed;

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
    Renderer::DrawSprite(*state, position, Vector2(150, 150), std::min(frame, state_frames), animation_direction == left);
}

void Player::Animation(){
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000/animation_speed){
        frame += 1;
        if (frame >= state_frames) frame = 0;
        animation_delay = currentTicks;
    }
}