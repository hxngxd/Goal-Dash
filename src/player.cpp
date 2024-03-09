#include "game.h"
#include "inputhandler.h"

void Player::Init(std::string name, Vector2 position){
    this->name = name;
    this->position = position;
    this->size = Vector2(48);
    this->currentFrame = 0;
    this->maxFrames = 0;
    this->animation_delay = 0;
    this->current_state = IDLE;
    this->previous_state = IDLE;
    this->direction = RIGHT;
    this->key_right = this->key_left = this->key_down = this->key_up = 0;
    this->isMovingSound = false;
}

void Player::Update(){
    MoveRightLeft();
    // MoveDownUp();
    Collision();
    Jump();
    DrawBox();
    Animation();
}

void Player::Animation(){
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000/Game::animation_speed){
        currentFrame += 1;
        if (currentFrame >= maxFrames) currentFrame = 0;
        animation_delay = currentTicks;
    }
    Sprite * current = nullptr;
    switch (current_state){
        case IDLE:
            current = Sprites["idle"];
            break;
        case RUN:
            current = Sprites["run"];
            break;
        case JUMP:
            current = Sprites["jump"];
            break;
    }
    maxFrames = current->maxFrames;
    Screen::DrawSprite(*current, position, Screen::player_size, std::min(currentFrame, maxFrames), (direction==LEFT));
}

void Player::MoveRightLeft(){
    if (!(key_left ^ key_right)){
        // std::cout << "Player stopped" << std::endl;
        if (current_state != JUMP) current_state = IDLE;
        if (velocity.l < 0) velocity.l += Game::player_acceleration*1.5;
        else if (velocity.l > 0) velocity.l = 0;
        if (velocity.r > 0) velocity.r -= Game::player_acceleration*1.5;
        else if (velocity.r < 0) velocity.r = 0;
    }
    else{
        if (key_left){
            // std::cout << "Player moved left" << std::endl;
            current_state = RUN;
            direction = LEFT;
            velocity.l -= Game::player_acceleration;
            velocity.l = velocity.l < -1 ? -1 : velocity.l;
            if (velocity.r > 0) velocity.r -= Game::player_acceleration*1.5;
            else if (velocity.l < 0) velocity.r = 0;
        }
        if (key_right){
            // std::cout << "Player moved right" << std::endl;
            current_state = RUN;
            direction = RIGHT;
            if (velocity.l < 0) velocity.l += Game::player_acceleration*1.5;
            else if (velocity.l > 0) velocity.l = 0;
            velocity.r += Game::player_acceleration;
            velocity.r = velocity.r > 1 ? 1 : velocity.r;
        }
    }
    position.x += (velocity.l + velocity.r) * Game::player_move_speed;
    
    Screen::bg_star_position.x -= (velocity.l + velocity.r) * 0.5;
    Screen::bg_star_position.x = clamp(Screen::bg_star_position.x, -Screen::bg_margin.x, 0);

    Screen::bg_cloud_position.x -= (velocity.l + velocity.r) * 0.25;
    Screen::bg_cloud_position.x = clamp(Screen::bg_cloud_position.x, -Screen::bg_margin.x, 0);


    if (velocity.l + velocity.r != 0){
        if (!isMovingSound && collide_down){
            playSound("run", run_channel, -1);
            isMovingSound = true;
        }
        if (!collide_down && isMovingSound){
            stopSound(run_channel);
            isMovingSound = false;
        }
    }
    else{
        if (isMovingSound){
            stopSound(run_channel);
            isMovingSound = false;
        }
    }
}

void Player::MoveDownUp(){
    if (!(key_up ^ key_down)){
        // std::cout << "Player stopped" << std::endl;
        current_state = IDLE;
        velocity.u = velocity.d = 0;
    }
    else{ 
        if (key_up){
            // std::cout << "Player moved up" << std::endl;
            current_state = RUN;
            velocity.u -= Game::player_acceleration;
            velocity.u = velocity.u < -1 ? -1 : velocity.u;
            velocity.d = 0;
        }
        if (key_down){
            // std::cout << "Player moved down" << std::endl;
            current_state = RUN;
            velocity.u = 0;
            velocity.d += Game::player_acceleration;
            velocity.d = velocity.d > 1 ? 1 : velocity.d;
        }
    }
    position.y += (velocity.u + velocity.d) * Game::player_move_speed;
}

void Player::Collision(){
    Vector2 playerCenter = Rect::getCenter(position, size);
    Vector2 playerCenterTile = IntV2(playerCenter/(Screen::player_size));

    std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> visit;
    std::queue<Vector2> Q;
    visit.insert({playerCenterTile, 1});
    Q.push(playerCenterTile);

    float maxDist = Screen::player_size*sqrt(61)/6-3;
    float eps = 1e-2;
    tmp_collide_down = tmp_collide_up = false;
    while (!Q.empty()){
        Vector2 u = Q.front();
        Q.pop();
        GameObject::BFS_Collision(*this, playerCenter, u + v2down, visit, Q, maxDist, eps);
        GameObject::BFS_Collision(*this, playerCenter, u + v2up, visit, Q, maxDist, eps);
        GameObject::BFS_Collision(*this, playerCenter, u + v2right, visit, Q, maxDist, eps);
        GameObject::BFS_Collision(*this, playerCenter, u + v2left, visit, Q, maxDist, eps);
    }
    collide_down = tmp_collide_down;
    collide_up = tmp_collide_up;
}

void Player::Jump(){
    if (!collide_down){
        if (collide_up) velocity.d = 0;
        velocity.d += Game::gravity;
        position.y += velocity.d;
        Screen::bg_star_position.y -= velocity.d/20;
        Screen::bg_star_position.y = clamp(Screen::bg_star_position.y, -Screen::bg_margin.y, 0);

        Screen::bg_cloud_position.y -= velocity.d/40;
        Screen::bg_cloud_position.y = clamp(Screen::bg_cloud_position.y, -Screen::bg_margin.y, 0);
    }
    else{
        if (current_state == JUMP){
            current_state = previous_state;
            previous_state = JUMP;
        }
        velocity.d = 0;
    }
}

void Player::DrawBox(){
    Vector2 startPos(position.x + Screen::player_size/6, position.y);
    Vector2 endPos(position.x + Screen::player_size/6*5, position.y + Screen::player_size);
    Screen::SetDrawColor(Color::white(255));
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, startPos.x, endPos.y);
}