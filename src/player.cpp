#include "game.h"
#include "inputhandler.h"

Player::Player(const char * name, Vector2 position){
    this->name = name;
    this->position = position;
    this->size = Vector2(48);
    this->currentFrame = 0;
    this->maxFrame = 0;
    this->animation_delay = 0;
    this->current_state = idle;
    this->previous_state = idle;
    this->direction = right;
    this->key_right = this->key_left = this->key_down = this->key_up = 0;
}

void Player::Update(){
    Movement();
    Animation();
}

void Player::Animation(){
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000/Game::animation_speed){
        currentFrame += 1;
        if (currentFrame >= maxFrame) currentFrame = 0;
        animation_delay = currentTicks;
    }
    Sprite * current = nullptr;
    switch (current_state){
        case idle:
            current = &Sprite::SpriteList[idle];
            break;
        case run:
            current = &Sprite::SpriteList[run];
            break;
        case jump:
            current = &Sprite::SpriteList[jump];
            break;
    }
    maxFrame = current->maxFrames;
    Renderer::DrawSprite(*current, position, Screen::player_size, std::min(currentFrame, maxFrame), (direction==left));
}

void Player::Movement(){
    if (!(key_left ^ key_right)){
        // std::cout << "Player stopped" << std::endl;
        if (current_state != jump) current_state = idle;
        if (velocity.l < 0) velocity.l += Game::player_acceleration_rate*1.5;
        else if (velocity.l > 0) velocity.l = 0;
        if (velocity.r > 0) velocity.r -= Game::player_acceleration_rate*1.5;
        else if (velocity.r < 0) velocity.r = 0;
    }
    else{
        if (key_left){
            // std::cout << "Player moved left" << std::endl;
            current_state = run;
            direction = left;
            velocity.l -= Game::player_acceleration_rate;
            velocity.l = velocity.l < -1 ? -1 : velocity.l;
            if (velocity.r > 0) velocity.r -= Game::player_acceleration_rate*1.5;
            else if (velocity.l < 0) velocity.r = 0;
        }
        if (key_right){
            // std::cout << "Player moved right" << std::endl;
            current_state = run;
            direction = right;
            if (velocity.l < 0) velocity.l += Game::player_acceleration_rate*1.5;
            else if (velocity.l > 0) velocity.l = 0;
            velocity.r += Game::player_acceleration_rate;
            velocity.r = velocity.r > 1 ? 1 : velocity.r;
        }
    }

    // if (!(key_up ^ key_down)){
    //     // std::cout << "Player stopped" << std::endl;
    //     current_state = idle;
    //     velocity.u = velocity.d = 0;
    // }
    // else{ 
    //     if (key_up){
    //         // std::cout << "Player moved up" << std::endl;
    //         current_state = run;
    //         velocity.u -= Game::player_acceleration_rate;
    //         velocity.u = velocity.u < -1 ? -1 : velocity.u;
    //         velocity.d = 0;
    //     }
    //     if (key_down){
    //         // std::cout << "Player moved down" << std::endl;
    //         current_state = run;
    //         velocity.u = 0;
    //         velocity.d += Game::player_acceleration_rate;
    //         velocity.d = velocity.d > 1 ? 1 : velocity.d;
    //     }
    // }
    // position.y += (velocity.u + velocity.d) * Game::player_moving_speed;

    position.x += (velocity.l + velocity.r) * Game::player_moving_speed;


    Vector2 playerCenter = Rect::RectCenter(position, size);
    Vector2 playerCenterTile = Vector2::Int(playerCenter/(Screen::player_size));

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
        GameObject::BFS_Collision(*this, playerCenter, u + Vector2::down, visit, Q, maxDist, eps);
        GameObject::BFS_Collision(*this, playerCenter, u + Vector2::up, visit, Q, maxDist, eps);
        GameObject::BFS_Collision(*this, playerCenter, u + Vector2::right, visit, Q, maxDist, eps);
        GameObject::BFS_Collision(*this, playerCenter, u + Vector2::left, visit, Q, maxDist, eps);
    }
    collide_down = tmp_collide_down;
    collide_up = tmp_collide_up;

    if (!collide_down){
        if (collide_up) velocity.d = 0;
        velocity.d += Game::gravity;
        position.y += velocity.d;
    }
    else{
        if (current_state == jump){
            current_state = previous_state;
            previous_state = jump;
        }
        velocity.d = 0;
    }

    Vector2 startPos(position.x + Screen::player_size/6, position.y);
    Vector2 endPos(position.x + Screen::player_size/6*5, position.y + Screen::player_size);
    Renderer::SetDrawColor(Color::white(255));
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, startPos.x, endPos.y);
}