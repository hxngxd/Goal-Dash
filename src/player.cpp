#include "game.h"

Player::Player(Vector2 position)
{
    this->name = "";
    this->position = position;
    this->size = Vector2(Screen::tile_size);
    this->scale = 0;
    this->animation_speed = 15;
    this->currentFrame = 0;
    this->maxFrames = 0;
    this->animation_delay = 0;
    this->current_state = this->previous_state = IDLE;
    this->direction = RIGHT;
    this->key_right = this->key_left = this->key_down = this->key_up = 0;
    memset(isDamaged, 0, sizeof(isDamaged));
}

void Player::Update()
{
    Animation();

    MoveRightLeft();

    if (Game::Properties["no_gravity"].b)
        MoveDownUp();
    else
        Jump();

    Collision();

    if (Game::Properties["draw_ray"].b)
        DrawBox();
}

void Player::Animation()
{
    float currentTicks = SDL_GetTicks();
    if (currentTicks > animation_delay + 1000 / animation_speed)
    {
        currentFrame += 1;
        if (currentFrame >= maxFrames)
            currentFrame = 0;
        animation_delay = currentTicks;
    }

    Sprite *current = nullptr;
    switch (current_state)
    {
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

    Screen::DrawSprite(*current, position, size, scale, std::min(currentFrame, maxFrames), (direction == LEFT));

    if (!Game::Properties["immortal"].b)
    {
        if (isDamaged[1] || isDamaged[2])
            Damaged(true);
        else
            Damaged(false);
    }
}

void Player::MoveRightLeft()
{
    if (!(key_left ^ key_right))
    {
        if (current_state != JUMP)
        {
            if (!collide_down.second)
                current_state = JUMP;
            else
                current_state = IDLE;
        }
        else
        {
            if (collide_down.second)
                current_state = IDLE;
        }

        if (velocity.l < 0)
            velocity.l += Game::Properties["player_acceleration"].f * 1.5;
        else if (velocity.l > 0)
            velocity.l = 0;

        if (velocity.r > 0)
            velocity.r -= Game::Properties["player_acceleration"].f * 1.5;
        else if (velocity.r < 0)
            velocity.r = 0;
    }
    else
    {
        if (key_left)
        {
            if (current_state != JUMP)
                current_state = RUN;

            direction = LEFT;

            velocity.l -= Game::Properties["player_acceleration"].f;
            velocity.l = velocity.l < -1 ? -1 : velocity.l;

            if (velocity.r > 0)
                velocity.r -= Game::Properties["player_acceleration"].f * 1.5;
            else if (velocity.l < 0)
                velocity.r = 0;
        }
        if (key_right)
        {
            if (current_state != JUMP)
                current_state = RUN;

            direction = RIGHT;

            if (velocity.l < 0)
                velocity.l += Game::Properties["player_acceleration"].f * 1.5;
            else if (velocity.l > 0)
                velocity.l = 0;

            velocity.r += Game::Properties["player_acceleration"].f;
            velocity.r = velocity.r > 1 ? 1 : velocity.r;
        }
    }

    float vx = (velocity.l + velocity.r);

    position.x += vx * Game::Properties["player_move_speed"].f;

    if (Game::Properties["moving_background"].b)
    {
        Background::Move(Vector2(-vx, 0), 0, 0.25);
        Background::Move(Vector2(-vx, 0), 1, 0.5);
        Background::Move(Vector2(-vx, 0), 2, 0.75);
    }

    if (velocity.l + velocity.r != 0)
    {
        if (!collide_down.second)
        {
            if (Mix_Playing(run_channel))
                stopSound(run_channel);
        }
        else
        {
            if (!Mix_Playing(run_channel))
            {
                if (Game::Properties["sound"].b)
                    playSound("run", run_channel, -1);
            }
        }
    }
    else
    {
        if (Mix_Playing(run_channel))
            stopSound(run_channel);
    }
}

void Player::MoveDownUp()
{
    if (!(key_up ^ key_down))
        velocity.u = velocity.d = 0;
    else
    {
        if (key_up)
        {
            velocity.u -= Game::Properties["player_acceleration"].f;
            velocity.u = velocity.u < -1 ? -1 : velocity.u;
            velocity.d = 0;
        }
        if (key_down)
        {
            velocity.u = 0;
            velocity.d += Game::Properties["player_acceleration"].f;
            velocity.d = velocity.d > 1 ? 1 : velocity.d;
        }
    }

    position.y += (velocity.u + velocity.d) * Game::Properties["player_move_speed"].f;
}

void Player::Collision()
{
    if (!Game::Properties["collision"].b)
    {
        position.x = clamp(position.x, -size.x / 6, Screen::resolution.x - size.x);
        position.y = clamp(position.y, 0, Screen::resolution.y - size.y);
        return;
    }

    Vector2 playerCenterTile = IntV2(Rect::getCenter(position, size) / Screen::tile_size);
    std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> visit;
    std::queue<Vector2> Q;
    visit.insert({playerCenterTile, 1});
    Q.push(playerCenterTile);

    collide_down.first = collide_up.first = false;
    isDamaged[0] = false;

    while (!Q.empty())
    {
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

void Player::MapCollision(Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> &visit,
                          std::queue<Vector2> &Q)
{
    float maxDist = Screen::tile_size * sqrt(61) / 6 - 3;
    float eps = 0;

    Vector2 playerCenter = Rect::getCenter(position, size);
    Vector2 nextCenter = (nextTile + Vector2(0.5)) * Screen::tile_size;
    float h = playerCenter.distance(nextCenter);

    if (IsV2InRange(nextTile, Vector2(), Vector2(15)) && visit.find(nextTile) == visit.end() && h <= maxDist)
    {
        int type = TileMap[nextTile.y][nextTile.x].first;
        if (type & (WALL | DAMAGE))
        {
            Vector2 d = nextCenter - playerCenter;
            float cos = d.x / h;
            float angle = acos(cos);
            float range = atan(6.0 / 5.0);
            if ((angle < range - eps) || (abs(cos - 1) <= eps))
            {
                position.x = std::min(position.x, (float)(nextTile.x - 5.0 / 6.0) * size.x - 1);
            }
            else if ((M_PI - range + eps < angle && angle < M_PI - eps) || (abs(cos + 1) <= eps))
            {
                position.x = std::max(position.x, (float)(nextTile.x + 5.0 / 6.0) * size.x + 1);
            }
            else if ((nextCenter.y > playerCenter.y) &&
                     ((range + eps * 5 < angle && angle < M_PI - range - eps * 5) || (abs(cos) <= eps)))
            {
                position.y = std::min(position.y, (float)(nextTile.y - 1) * size.y);
                if (abs(playerCenter.y - nextCenter.y) - size.y <= 3)
                    collide_down.first = true;
            }
            else if ((nextCenter.y < playerCenter.y) &&
                     ((range + eps * 5 < angle && angle < M_PI - range - eps * 5) || (abs(cos) <= eps)))
            {
                position.y = std::max(position.y, (float)(nextTile.y + 1) * size.y);
                if (abs(playerCenter.y - nextCenter.y) - size.y <= 0)
                    collide_up.first = true;
            }

            if (Game::Properties["draw_ray"].b)
            {
                if (type & WALL)
                    Screen::SetDrawColor(Color::white(255));
                else
                    Screen::SetDrawColor(Color::red(255));
            }

            if (!Game::Properties["immortal"].b && type & DAMAGE)
            {
                if (Rect::isCollide(playerCenter, Vector2(size.x / 6 * 4, size.y), nextCenter,
                                    Vector2(Screen::tile_size), 3))
                    isDamaged[0] = true;
            }
        }
        else if (type & COIN)
        {
            if (Game::Properties["draw_ray"].b)
                Screen::SetDrawColor(Color::yellow(255));

            if (Rect::isCollide(playerCenter, Vector2(size.x / 6 * 4, size.y), nextCenter, Vector2(Screen::tile_size),
                                0))
            {
                Game::Properties["player_score"].i++;
                TileMap[nextTile.y][nextTile.x].first = 0;
                delete TileMap[nextTile.y][nextTile.x].second;
                TileMap[nextTile.y][nextTile.x].second = nullptr;

                if (Game::Properties["sound"].b)
                    playSound("coin", coin_channel, 0);
            }
        }
        else if (type & WIN)
        {
            if (Game::Properties["draw_ray"].b)
                Screen::SetDrawColor(Color::green(255));

            if (Rect::isCollide(playerCenter, Vector2(size.x / 6 * 4, size.y), nextCenter, Vector2(Screen::tile_size),
                                0) &&
                !Game::Properties["player_won"].b && Game::Properties["player_score"].i == Game::Properties["coin"].i)
            {
                ShowMsg(0, logging, "player won!");
                Game::Properties["player_won"].b = 1;
                DelayFunction::CreateDelayFunction(250, []() {
                    Game::scene->DeleteScene();
                    return 1;
                });
            }
        }
        else if (type & SPAWN)
        {
            if (Game::Properties["draw_ray"].b)
                Screen::SetDrawColor(Color::cyan(255));
        }

        if (Game::Properties["draw_ray"].b && type)
        {
            SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, nextCenter.x, nextCenter.y);
        }

        visit.insert({nextTile, 1});
        Q.push(nextTile);
    }
}

void Player::Jump()
{
    if (!collide_down.second)
    {
        if (collide_up.second && velocity.d < -1)
            velocity.d = 0;

        velocity.d += Game::Properties["gravity"].f;
        position.y += velocity.d;

        if (Game::Properties["moving_background"].b)
        {
            Background::Move(Vector2(0, -velocity.d), 0, 0.1);
            Background::Move(Vector2(0, -velocity.d), 1, 0.15);
            Background::Move(Vector2(0, -velocity.d), 2, 0.2);
        }
    }
    else
    {
        if (current_state == JUMP)
        {
            current_state = previous_state;
            previous_state = JUMP;
        }

        if (velocity.d > 11)
        {
            if (!Game::Properties["immortal"].b)
            {
                isDamaged[2] = true;
                auto notDamage = [](Player *player) {
                    player->isDamaged[2] = false;
                    return 1;
                };
                DelayFunction::CreateDelayFunction(500, std::bind(notDamage, this));
            }

            if (Game::Properties["sound"].b)
                playSound("fall", fall_channel, 0);
        }

        velocity.d = 0;
    }
}

void Player::DrawBox()
{
    Vector2 startPos(position.x + size.x / 6, position.y);
    Vector2 endPos(position.x + size.x / 6 * 5, position.y + size.y);
    Screen::SetDrawColor(Color::white(255));

    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, startPos.x, startPos.y, startPos.x, endPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, endPos.x, startPos.y);
    SDL_RenderDrawLine(Game::renderer, endPos.x, endPos.y, startPos.x, endPos.y);
}

void Player::Damaged(bool c)
{
    Uint8 r = 255, g = 114, b = 118;
    if (!c)
    {
        r = g = b = 255;
    }
    SDL_SetTextureColorMod(Sprites["idle"]->texture, r, g, b);
    SDL_SetTextureColorMod(Sprites["run"]->texture, r, g, b);
    SDL_SetTextureColorMod(Sprites["jump"]->texture, r, g, b);
}