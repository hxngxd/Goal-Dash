#include "game.h"

void Player::Init(const char * name, Vector2 position){
    this->name = name;
    this->position = position;
    this->size = Vector2(48);
    this->_idle.LoadSprite("img/idle.png", 10, this->size);
    this->_run.LoadSprite("img/run.png", 9, this->size);
    this->_jump.LoadSprite("img/jump.png", 4, this->size);
    this->currentFrame = 0;
    this->maxFrame = 0;
    this->animation_delay = 0;
}

void Player::Update(){
    Movement();
    Animation();
}

void Player::Animation(){
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000/animation_speed){
        currentFrame += 1;
        if (currentFrame >= maxFrame) currentFrame = 0;
        animation_delay = currentTicks;
    }
    Sprite * current_state_sprite = nullptr;
    switch (current_state){
        case idle:
            current_state_sprite = &_idle;
            break;
        case run:
            current_state_sprite = &_run;
            break;
        case jump:
            current_state_sprite = &_jump;
            break;
    }
    maxFrame = current_state_sprite->maxFrames;
    Renderer::DrawSprite(*current_state_sprite, position, Vector2(player_size), std::min(currentFrame, maxFrame), (direction==left));
}

void Player::Movement(){
    if (!(KeyboardHandler::key_a ^ KeyboardHandler::key_d)){
        // std::cout << "Player stopped" << std::endl;
        if (current_state != jump) current_state = idle;
        velocity.l = velocity.r = 0;
    }
    else{
        if (KeyboardHandler::key_a){
            // std::cout << "Player moved left" << std::endl;
            current_state = run;
            direction = left;
            velocity.l -= player_acceleration_rate;
            velocity.l = velocity.l < -1 ? -1 : velocity.l;
            velocity.r = 0;
        }
        if (KeyboardHandler::key_d){
            // std::cout << "Player moved right" << std::endl;
            current_state = run;
            direction = right;
            velocity.l = 0;
            velocity.r += player_acceleration_rate;
            velocity.r = velocity.r > 1 ? 1 : velocity.r;
        }
    }

    // if (!(KeyboardHandler::key_w ^ KeyboardHandler::key_s)){
    //     // std::cout << "Player stopped" << std::endl;
    //     current_state = idle;
    //     velocity.u = velocity.d = 0;
    // }
    // else{ 
    //     if (KeyboardHandler::key_w){
    //         // std::cout << "Player moved up" << std::endl;
    //         current_state = run;
    //         velocity.u -= player_acceleration_rate;
    //         velocity.u = velocity.u < -1 ? -1 : velocity.u;
    //         velocity.d = 0;
    //     }
    //     if (KeyboardHandler::key_s){
    //         // std::cout << "Player moved down" << std::endl;
    //         current_state = run;
    //         velocity.u = 0;
    //         velocity.d += player_acceleration_rate;
    //         velocity.d = velocity.d > 1 ? 1 : velocity.d;
    //     }
    // }

    position.x += (velocity.l + velocity.r) * player_moving_speed;
    // position.y += (velocity.u + velocity.d) * player_moving_speed;

    float eps = 1e-2;
    bool touch_the_ground = false;
    bool touch_the_ceiling = false;

    for (auto tile : MapTile::Tiles){
        Vector2 tileCenter = tile.position + tile.size/2;
        Vector2 playerCenter = position + size/2;
        bool is_near_player = playerCenter.distance(tileCenter) <= playerCenter.distance(playerCenter + Vector2(size.x/6*5, size.y)) - 1;
        if (is_near_player){
            SDL_RenderDrawLine(renderer, playerCenter.x, playerCenter.y, tileCenter.x, tileCenter.y);
            Vector2 d = tileCenter - playerCenter;
            float h = playerCenter.distance(tileCenter);
            float cos = d.x/h;
            float angle = (tileCenter.y <= playerCenter.y ? 1 : -1) * std::acos(cos);
            int where = 0;
            float angle_range = std::atan(6.0/5.0);
            if ((abs(angle) < angle_range - eps) || (abs(cos-1) <= eps)){
                where = 1; //right
            }
            else if ((M_PI - angle_range + eps < abs(angle) && abs(angle) < M_PI - eps) || (abs(cos+1) <= eps)){
                where = 2; //left
            }
            else if ((-M_PI + angle_range + eps*7 < angle && angle < - angle_range - eps*7) || (abs(cos) <= eps && tileCenter.y > playerCenter.y)){
                where = 3; //down
            }
            else if ((angle_range + eps*7 < angle && angle < M_PI - angle_range - eps*7) || (abs(cos) <= eps && tileCenter.y < playerCenter.y)){
                where = 4; //up
            }
            if (GameObject::isCollide(playerCenter, Vector2(size.x/6*4, size.y), tileCenter, tile.size, 0)){
                switch (where){
                    case 1:
                        // std::cout << "Right" << std::endl;
                        position.x = tile.position.x - size.x/6*5;
                        break;
                    case 2:
                        // std::cout << "Left" << std::endl;
                        position.x = tile.position.x + size.x/6*5;
                        break;
                    case 3:
                        // std::cout << "Under" << std::endl;
                        position.y = tile.position.y - size.y;
                        touch_the_ground = true;
                        break;
                    case 4:
                        // std::cout << "Above" << std::endl;
                        position.y = tile.position.y + size.y;
                        touch_the_ceiling = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (!touch_the_ground){
        if (touch_the_ceiling) velocity.d = 0;
        velocity.d += gravity;
        position.y += velocity.d;
    }
    else{
        if (KeyboardHandler::key_space && current_state != jump && !touch_the_ceiling){
            previous_state = current_state;
            current_state = jump;
            velocity.d = -jump_speed;
            position.y += velocity.d;
        }
        else if (current_state == jump){
            current_state = previous_state;
            previous_state = jump;
        }
        else{
            velocity.d = 0;
        }
    }

    Vector2 startPos(position.x + player_size/6, position.y);
    Vector2 endPos(position.x + player_size/6*5, position.y + player_size);
    SDL_RenderDrawLine(renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(renderer, endPos.x, endPos.y, startPos.x, endPos.y);

    // int tile_size = resolution.x/map_size;

    // Vector2 startTile(
    //     (startPos.x + player_speed)/tile_size,
    //     (startPos.y + player_speed)/tile_size
    // );

    // Vector2 endTile(
    //     (endPos.x - player_speed)/tile_size,
    //     (endPos.y - player_speed)/tile_size
    // );

    // if (animation_state == run || (animation_state == jump && previous_animation_state == run)){
    //     if (animation_direction == left){
    //         velocity.l -= player_acceleration_rate;
    //         velocity.l = (velocity.l < -1 ? -1 : velocity.l);
    //         velocity.r = 0;
    //     }
    //     else{
    //         velocity.l = 0;
    //         velocity.r += player_acceleration_rate;
    //         velocity.r = (velocity.r > 1 ? 1 : velocity.r);
    //     }
    // }
    // else{
    //     velocity.l = velocity.r = 0;
    // }

    // position.x += (velocity.l + velocity.r) * player_speed;

    // for (int i=std::max(0, (int)startTile.y);i<=std::min(map_size-1, (int)endTile.y);i++){
    //     if (startTile.x > 0 && tileMap[i][startTile.x-1]){
    //         if (position.x + player_size/6 - (int)startTile.x*tile_size <= 0){
    //             position.x -= velocity.l * player_speed;
    //         }
    //     }
    // }

    // for (int i=std::max(0, (int)startTile.y);i<=std::min(map_size-1, (int)endTile.y);i++){
    //     if (endTile.x < map_size - 1 && tileMap[i][endTile.x+1]){
    //         if (((int)endTile.x+1)*tile_size - position.x - player_size/6*5 <= 0){
    //             position.x -= velocity.r * player_speed;
    //         }
    //     }
    // }

    // for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
    //     if (startTile.y > 0 && tileMap[startTile.y-1][i]){
    //         if (position.y - (int)startTile.y*tile_size <= 0){
    //             position.y -= velocity.u * player_speed;
    //         }
    //     }
    // }

    // if (!onGround){
    //     velocity.d += gravity;
    //     position.y += velocity.d;
    //     for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
    //         if (endTile.y < map_size - 1 && tileMap[endTile.y+1][i]){
    //             if (((int)endTile.y+1)*tile_size - position.y - player_size <= 0){
    //                 position.y = ((int)endTile.y+1)*tile_size - player_size;
    //                 onGround = true;
    //                 velocity.d = 0;
    //                 animation_state = previous_animation_state;
    //             }
    //         }
    //     }
    //     for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
    //         if (startTile.y > 0 && tileMap[startTile.y-1][i]){
    //             if (position.y - (int)startTile.y*tile_size <= 0){
    //                 position.y = (int)startTile.y*tile_size;
    //                 velocity.d = 0;
    //             }
    //         }
    //     }
    // }
    // else{
    //     int tmp = false;
    //     for (int i=std::max(0, (int)startTile.x);i<=std::min(map_size-1, (int)endTile.x);i++){
    //         if (endTile.y < map_size - 1 && tileMap[endTile.y+1][i]){
    //             if (((int)endTile.y+1)*tile_size - position.y - player_size <= 0){
    //                 tmp = true;
    //             }
    //         }
    //     }
    //     onGround = tmp;
    // }

    // std::cout << onGround << std::endl;
}