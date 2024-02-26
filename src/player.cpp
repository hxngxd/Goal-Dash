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

    if (!(key_up ^ key_down)){
        // std::cout << "Player stopped" << std::endl;
        current_state = idle;
        velocity.u = velocity.d = 0;
    }
    else{ 
        if (key_up){
            // std::cout << "Player moved up" << std::endl;
            current_state = run;
            velocity.u -= Game::player_acceleration_rate;
            velocity.u = velocity.u < -1 ? -1 : velocity.u;
            velocity.d = 0;
        }
        if (key_down){
            // std::cout << "Player moved down" << std::endl;
            current_state = run;
            velocity.u = 0;
            velocity.d += Game::player_acceleration_rate;
            velocity.d = velocity.d > 1 ? 1 : velocity.d;
        }
    }

    position.x += (velocity.l + velocity.r) * Game::player_moving_speed;
    position.y += (velocity.u + velocity.d) * Game::player_moving_speed;

    Vector2 playerCenter = Rect::RectCenter(position, size);
    Vector2 playerCenterTile = playerCenter/(Screen::player_size);
    Vector2::Int(playerCenterTile);

    float maxDist = 48*3;
    std::vector<std::vector<bool>> trace(Screen::map_size, std::vector<bool>(Screen::map_size, 0));
    std::queue<Vector2> Q;
    trace[playerCenterTile.x][playerCenterTile.y] = 1;
    Q.push(playerCenterTile);

    Renderer::SetDrawColor(Color::cyan(255));

    while (!Q.empty()){
        Vector2 u = Q.front();
        Q.pop();
        Vector2 rightTile = u + Vector2::right;
        Vector2 rightCenter = (rightTile + Vector2(0.5)) * Screen::player_size;
        Vector2 leftTile = u + Vector2::left;
        Vector2 leftCenter = (leftTile + Vector2(0.5)) * Screen::player_size;
        Vector2 downTile = u + Vector2::down;
        Vector2 downCenter = (downTile + Vector2(0.5)) * Screen::player_size;
        Vector2 upTile = u + Vector2::up;
        Vector2 upCenter = (upTile + Vector2(0.5)) * Screen::player_size;

        if (Vector2::IsInRange(rightTile, 0, 15, 0, 15) &&
            !trace[rightTile.x][rightTile.y] &&
            playerCenter.distance(rightCenter) <= maxDist)
        {
            if (Game::map[rightTile.y][rightTile.x]){
                SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, rightCenter.x, rightCenter.y);
            }
            trace[rightTile.x][rightTile.y] = 1;
            Q.push(rightTile);
        }

        if (Vector2::IsInRange(leftTile, 0, 15, 0, 15) &&
            !trace[leftTile.x][leftTile.y] &&
            playerCenter.distance(leftCenter) <= maxDist)
        {
            if (Game::map[leftTile.y][leftTile.x]){
                SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, leftCenter.x, leftCenter.y);
            }
            trace[leftTile.x][leftTile.y] = 1;
            Q.push(leftTile);
        }

        if (Vector2::IsInRange(downTile, 0, 15, 0, 15) &&
            !trace[downTile.x][downTile.y] &&
            playerCenter.distance(downCenter) <= maxDist)
        {
            if (Game::map[downTile.y][downTile.x]){
                SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, downCenter.x, downCenter.y);
            }
            trace[downTile.x][downTile.y] = 1;
            Q.push(downTile);
        }

        if (Vector2::IsInRange(upTile, 0, 15, 0, 15) &&
            !trace[upTile.x][upTile.y] &&
            playerCenter.distance(upCenter) <= maxDist)
        {
            if (Game::map[upTile.y][upTile.x]){
                SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, upCenter.x, upCenter.y);
            }
            trace[upTile.x][upTile.y] = 1;
            Q.push(upTile);
        }
    }

//     float eps = 1e-2;
//     bool touch_the_ground = false;
//     bool touch_the_ceiling = false;

//     for (auto & tile : MapTile::Tiles){
//         Vector2 tileCenter = tile.position + tile.size/2;
//         bool is_near_player = playerCenter.distance(tileCenter) <= playerCenter.distance(playerCenter + Vector2(size.x/6*5, size.y)) - 1;
//         if (is_near_player){
//             // SDL_RenderDrawLine(renderer, playerCenter.x, playerCenter.y, tileCenter.x, tileCenter.y);
//             Vector2 d = tileCenter - playerCenter;
//             float h = playerCenter.distance(tileCenter);
//             float cos = d.x/h;
//             float angle = (tileCenter.y <= playerCenter.y ? 1 : -1) * std::acos(cos);
//             int where = 0;
//             float angle_range = std::atan(6.0/5.0);
//             if ((abs(angle) < angle_range - eps) || (abs(cos-1) <= eps)){
//                 where = 1; //right
//             }
//             else if ((M_PI - angle_range + eps < abs(angle) && abs(angle) < M_PI - eps) || (abs(cos+1) <= eps)){
//                 where = 2; //left
//             }
//             else if ((-M_PI + angle_range + eps*5 < angle && angle < - angle_range - eps*5) || (abs(cos) <= eps && tileCenter.y > playerCenter.y)){
//                 where = 3; //down
//             }
//             else if ((angle_range + eps*10 < angle && angle < M_PI - angle_range - eps*10) || (abs(cos) <= eps && tileCenter.y < playerCenter.y)){
//                 where = 4; //up
//             }
//             if (GameObject::isCollide(playerCenter, Vector2(size.x/6*4, size.y), tileCenter, tile.size, 0)){
//                 switch (tile.type){
//                     case 1:
//                         switch (where){
//                             case 1:
//                                 // std::cout << "Right" << std::endl;
//                                 position.x = tile.position.x - size.x/6*5;
//                                 break;
//                             case 2:
//                                 // std::cout << "Left" << std::endl;
//                                 position.x = tile.position.x + size.x/6*5;
//                                 break;
//                             case 3:
//                                 // std::cout << "Under" << std::endl;
//                                 position.y = tile.position.y - size.y;
//                                 touch_the_ground = true;
//                                 break;
//                             case 4:
//                                 // std::cout << "Above" << std::endl;
//                                 position.y = tile.position.y + size.y;
//                                 touch_the_ceiling = true;
//                                 break;
//                             default:
//                                 break;
//                         }
//                         break;
//                     case 2:
//                         tile.hidden = true;
//                         break;
//                     default:
//                         break;
//                 }
//             }
//         }
//     }

//     if (!touch_the_ground){
//         if (touch_the_ceiling) velocity.d = 0;
//         velocity.d += gravity;
//         position.y += velocity.d;
//     }
//     else{
//         if (KeyboardHandler::key_space && current_state != jump && !touch_the_ceiling){
//             previous_state = current_state;
//             current_state = jump;
//             velocity.d = -jump_speed;
//             position.y += velocity.d;
//         }
//         else if (current_state == jump){
//             current_state = previous_state;
//             previous_state = jump;
//         }
//         else{
//             velocity.d = 0;
//         }
//     }

    Vector2 startPos(position.x + Screen::player_size/6, position.y);
    Vector2 endPos(position.x + Screen::player_size/6*5, position.y + Screen::player_size);
    Renderer::SetDrawColor(Color::white(255));
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, startPos.x, endPos.y);
}