#include "game.h"

void Player::Init(std::string name){
    this->name = name;
    this->position = this->starting_position;
    this->size = Vector2(Screen::tile_size);
    this->scale = 0;
    this->animation_speed = 15;
    this->currentFrame = 0;
    this->maxFrames = 0;
    this->animation_delay = 0;
    this->current_state = this->previous_state = IDLE;
    this->direction = RIGHT;
    this->key_right = this->key_left = this->key_down = this->key_up = 0;
    this->move_speed = 5;
    this->acceleration = 0.04;
    this->jump_speed = 10;
    this->score = 0;
    memset(isDamaged, 0, sizeof(isDamaged));
}

void Player::Update(){
    if (SDL_GetTicks() <= wait_for_animation) return;
    MoveRightLeft();
    // MoveDownUp();
    Collision();
    Jump();
    DrawBox();
    Animation();
}

void Player::Animation(){
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000/animation_speed){
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
    Screen::DrawSprite(*current, position, size, scale, std::min(currentFrame, maxFrames), (direction==LEFT));
    incScale(scale);

    if (isDamaged[1] || isDamaged[2]) Damaged(true);
    else Damaged(false);
}

void Player::MoveRightLeft(){
    if (!(key_left ^ key_right)){
        // std::cout << "Player stopped" << std::endl;
        if (current_state != JUMP) current_state = IDLE;
        if (velocity.l < 0) velocity.l += acceleration * 1.5;
        else if (velocity.l > 0) velocity.l = 0;
        if (velocity.r > 0) velocity.r -= acceleration * 1.5;
        else if (velocity.r < 0) velocity.r = 0;
    }
    else{
        if (key_left){
            // std::cout << "Player moved left" << std::endl;
            if (current_state != JUMP) current_state = RUN;
            direction = LEFT;
            velocity.l -= acceleration;
            velocity.l = velocity.l < -1 ? -1 : velocity.l;
            if (velocity.r > 0) velocity.r -= acceleration * 1.5;
            else if (velocity.l < 0) velocity.r = 0;
        }
        if (key_right){
            // std::cout << "Player moved right" << std::endl;
             if (current_state != JUMP) current_state = RUN;
            direction = RIGHT;
            if (velocity.l < 0) velocity.l += acceleration * 1.5;
            else if (velocity.l > 0) velocity.l = 0;
            velocity.r += acceleration;
            velocity.r = velocity.r > 1 ? 1 : velocity.r;
        }
    }
    float vx = (velocity.l + velocity.r);

    position.x += vx * move_speed;
    
    Background::Move(Vector2(-vx, 0), 0, 0.25);
    Background::Move(Vector2(-vx, 0), 1, 0.5);
    Background::Move(Vector2(-vx, 0), 2, 0.75);

    if (velocity.l + velocity.r != 0){
        if (!collide_down.second){
            if (Mix_Playing(run_channel)){
                stopSound(run_channel);
            }
        }
        else{
            if (!Mix_Playing(run_channel)){
                playSound("run", run_channel, -1);
            }
        }
    }
    else{
        if (Mix_Playing(run_channel)){
            stopSound(run_channel);
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
            velocity.u -= acceleration;
            velocity.u = velocity.u < -1 ? -1 : velocity.u;
            velocity.d = 0;
        }
        if (key_down){
            // std::cout << "Player moved down" << std::endl;
            current_state = RUN;
            velocity.u = 0;
            velocity.d += acceleration;
            velocity.d = velocity.d > 1 ? 1 : velocity.d;
        }
    }
    position.y += (velocity.u + velocity.d) * move_speed;
}

void Player::Collision(){
    Vector2 playerCenterTile = IntV2(Rect::getCenter(position, size) / Screen::tile_size);
    std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> visit;
    std::queue<Vector2> Q;
    visit.insert({playerCenterTile, 1});
    Q.push(playerCenterTile);

    collide_down.first = collide_up.first = false;
    isDamaged[0] = false;
    while (!Q.empty()){
        Vector2 u = Q.front();
        Q.pop();
        MapCollision(u + v2down, visit, Q);
        MapCollision(u + v2up, visit, Q);
        MapCollision(u + v2right, visit, Q);
        MapCollision(u + v2left, visit, Q);
    }
    collide_down.second = collide_down.first;
    collide_up.second = collide_up.first;
    isDamaged[1] = isDamaged[0];
}

void Player::MapCollision(
    Vector2 nextTile,
    std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> & visit,
    std::queue<Vector2> & Q
){
    float maxDist = Screen::tile_size * sqrt(61) / 6 - 3;
    float eps = 1e-2;
    bool draw = true;
    Vector2 playerCenter = Rect::getCenter(position, size);
    Vector2 nextCenter = (nextTile + Vector2(0.5)) * Screen::tile_size;
    float h = playerCenter.distance(nextCenter);
    if (
        IsV2InRange(nextTile, Vector2(), Vector2(15)) &&
        visit.find(nextTile) == visit.end() &&
        h <= maxDist)
    {
        int type = TileMap[nextTile.y][nextTile.x];
        if (type & (WALL | DAMAGE)){
            Vector2 d = nextCenter - playerCenter;
            float cos = d.x/h;
            float angle = acos(cos);
            float range = atan(6.0/5.0);
            if (
                (angle < range - eps) ||
                (abs(cos-1) <= eps)
            ){
                position.x = std::min(position.x, (float)(nextTile.x - 5.0/6.0) * size.x - 1);
            }
            else if (
                (M_PI - range + eps < angle && angle < M_PI - eps) ||
                (abs(cos+1) <= eps)
            ){
                position.x = std::max(position.x, (float)(nextTile.x + 5.0/6.0) * size.x + 1);
            }
            else if (
                (nextCenter.y > playerCenter.y) &&
                ((range + eps * 5 < angle && angle < M_PI - range - eps * 5) || (abs(cos) <= eps))
            ){
                position.y = std::min(position.y, (float)(nextTile.y - 1) * size.y);
                if (abs(playerCenter.y - nextCenter.y) - size.y <= 3){
                    collide_down.first = true;
                }
            }
            else if (
                (nextCenter.y < playerCenter.y) &&
                ((range + eps * 5 < angle && angle < M_PI - range - eps * 5) || (abs(cos) <= eps))
            ){
                position.y = std::max(position.y, (float)(nextTile.y + 1) * size.y);
                if (abs(playerCenter.y - nextCenter.y) - size.y <= 0){
                    collide_up.first = true;
                }
            }
            if (draw){
                if (type & WALL) Screen::SetDrawColor(Color::white(255));
                else Screen::SetDrawColor(Color::red(255));
            }
            if (type & DAMAGE){
                if (Rect::isCollide(
                    playerCenter,
                    Vector2(size.x/6*4, size.y),
                    nextCenter,
                    Vector2(Screen::tile_size),
                    3)
                ){
                    isDamaged[0] = true;
                }
            }
        }
        else if (type & COIN){
            if (draw) Screen::SetDrawColor(Color::yellow(255));
            if (Rect::isCollide(
                    playerCenter,
                    Vector2(size.x/6*4, size.y),
                    nextCenter,
                    Vector2(Screen::tile_size),
                    0)
                ){
                score++;
                TileMap[nextTile.y][nextTile.x] = 0;
                playSound("coin", coin_channel, 0);
            }
        }
        else if (type & WIN){
            if (draw) Screen::SetDrawColor(Color::green(255));
            if (Rect::isCollide(
                playerCenter,
                Vector2(size.x/6*4, size.y),
                nextCenter,
                Vector2(Screen::tile_size),
                0)
            ){
                ShowMsg(0, logging, "player won!");
            }
        }
        else if (type & SPAWN){
            if (draw) Screen::SetDrawColor(Color::cyan(255));
        }
        if (draw && type){
            SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, nextCenter.x, nextCenter.y);
        }
        visit.insert({nextTile, 1});
        Q.push(nextTile);
    }
}

void Player::Jump(){
    if (!collide_down.second){
        if (collide_up.second) velocity.d = 0;
        velocity.d += Game::gravity;
        position.y += velocity.d;
        Background::Move(Vector2(0, -velocity.d), 0, 0.1);
        Background::Move(Vector2(0, -velocity.d), 1, 0.15);
        Background::Move(Vector2(0, -velocity.d), 2, 0.2);
    }
    else{
        if (current_state == JUMP){
            current_state = previous_state;
            previous_state = JUMP;
        }
        if (velocity.d > 11){
            isDamaged[2] = true;
            auto notDamage = [](Player * player){
                player->isDamaged[2] = false;
            };
            DelayFunction::CreateDelayFunction(500, std::bind(notDamage, this));
            playSound("fall", fall_channel, 0);
        }
        velocity.d = 0;
    }
}

void Player::DrawBox(){
    Vector2 startPos(position.x + size.x/6, position.y);
    Vector2 endPos(position.x + size.x/6*5, position.y + size.y);
    Screen::SetDrawColor(Color::white(255));
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, startPos.x, endPos.y);
}

void Player::Damaged(bool c){
    Uint8 r=255, g=114, b=118;
    if (!c){
        r = g = b = 255;
    }
    SDL_SetTextureColorMod(Sprites["idle"]->texture, r, g, b);
    SDL_SetTextureColorMod(Sprites["run"]->texture, r, g, b);
    SDL_SetTextureColorMod(Sprites["jump"]->texture, r, g, b);
}