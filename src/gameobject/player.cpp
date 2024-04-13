#include "../datalib/mixer.h"
#include "../datalib/sprite.h"
#include "../datalib/util.h"
#include "../event/ui.h"
#include "../game.h"
#include "gameobject.h"
#include "map.h"
#include <cstring>
#include <set>
#include <string>

int Player::total_score = 0;

Player::Player(Vector2 position)
{
    this->name = "";
    this->position = position;
    this->size = Vector2(Screen::tile_size);
    this->scale = 0;
    this->animation_speed = 15;
    this->current_frame = 0;
    this->max_frames = 0;
    this->animation_delay = 0;
    this->current_state = this->previous_state = IDLE;
    this->direction = RIGHT;
    this->key_right = this->key_left = this->key_down = this->key_up = 0;
    this->hp = 100;
    this->current_score = 0;
    this->win_score = Map::count_types[COIN];
    this->won = false;
}

Player::~Player()
{
    StopSound(CHANNEL_RUN);
}

void Player::Update()
{
    Animation();

    MoveRightLeft();

    if (Game::properties["gravity"].f == 0.0f)
        MoveDownUp();
    else
        Jump();

    Collision();

    DrawBox();
}

void Player::Animation()
{
    std::string current;
    bool isRight = direction == RIGHT;
    switch (current_state)
    {
    case IDLE:
        current = "idle";
        break;
    case RUN:
        current = "run";
        break;
    case JUMP:
        current = "jump";
        break;
    default:
        current = "idle";
        break;
    }
    Animate(this, current, !isRight);

    Vector2 startPos(position.x + size.x / 6, position.y);
    Vector2 endPos(position.x + size.x / 6 * 5, position.y + size.y);
    Vector2 centerPos = Rect::GetCenter(startPos, endPos - startPos);

    Vector2 hbSize = Vector2(size.x * 1.5, size.y / 5);
    Vector2 hbPos = centerPos - (hbSize / 2);
    hbPos.y -= size.y * 7 / 10;
    DrawSprite("healthbar", hbPos, hbSize, scale, (int)(4.0f - (float)hp / 25.0f));
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
            velocity.l += Game::properties["player_acceleration"].f * 1.5;
        else if (velocity.l > 0)
            velocity.l = 0;

        if (velocity.r > 0)
            velocity.r -= Game::properties["player_acceleration"].f * 1.5;
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

            velocity.l -= Game::properties["player_acceleration"].f;
            velocity.l = velocity.l < -1 ? -1 : velocity.l;

            if (velocity.r > 0)
                velocity.r -= Game::properties["player_acceleration"].f * 1.5;
            else if (velocity.l < 0)
                velocity.r = 0;
        }
        if (key_right)
        {
            if (current_state != JUMP)
                current_state = RUN;

            direction = RIGHT;

            if (velocity.l < 0)
                velocity.l += Game::properties["player_acceleration"].f * 1.5;
            else if (velocity.l > 0)
                velocity.l = 0;

            velocity.r += Game::properties["player_acceleration"].f;
            velocity.r = velocity.r > 1 ? 1 : velocity.r;
        }
    }

    float vx = (velocity.l + velocity.r);

    position.x += vx * Game::properties["player_move_speed"].f;

    Background::Move(Vector2(-vx, 0), 0, 0.25);
    Background::Move(Vector2(-vx, 0), 1, 0.5);
    Background::Move(Vector2(-vx, 0), 2, 0.75);

    if (velocity.l + velocity.r != 0)
    {
        if (!collide_down.second)
        {
            if (Mix_Playing(CHANNEL_RUN))
                StopSound(CHANNEL_RUN);
        }
        else
        {
            if (!Mix_Playing(CHANNEL_RUN))
            {
                if (Game::properties["sound"].b)
                    PlaySound("run", CHANNEL_RUN, -1);
            }
        }
    }
    else
    {
        if (Mix_Playing(CHANNEL_RUN))
            StopSound(CHANNEL_RUN);
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
            velocity.u -= Game::properties["player_acceleration"].f;
            velocity.u = velocity.u < -1 ? -1 : velocity.u;
            velocity.d = 0;
        }
        if (key_down)
        {
            velocity.u = 0;
            velocity.d += Game::properties["player_acceleration"].f;
            velocity.d = velocity.d > 1 ? 1 : velocity.d;
        }
    }

    position.y += (velocity.u + velocity.d) * Game::properties["player_move_speed"].f;
}

void Player::Collision()
{
    if (!Game::properties["player_collision"].b)
    {
        position.x = Clamp(position.x, -size.x / 6, Screen::resolution.x - size.x);
        position.y = Clamp(position.y, 0.0f, Screen::resolution.y - size.y);
        return;
    }

    Vector2 playerCenterTile = Int(Rect::GetCenter(position, size) / Screen::tile_size);
    std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> visit;
    std::queue<Vector2> Q;
    visit.insert({playerCenterTile, 1});
    Q.push(playerCenterTile);

    collide_down.first = collide_up.first = false;

    while (!Q.empty())
    {
        Vector2 u = Q.front();
        Q.pop();

        MapCollision(u + Vector2::Down, visit, Q);
        MapCollision(u + Vector2::Up, visit, Q);
        MapCollision(u + Vector2::Right, visit, Q);
        MapCollision(u + Vector2::Left, visit, Q);
    }

    collide_down.second = collide_down.first;
    collide_up.second = collide_up.first;
}

void Player::MapCollision(Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> &visit,
                          std::queue<Vector2> &Q)
{
    float maxDist = (Screen::tile_size * sqrt(61) / 6 - 3) * Game::properties["player_collision_dist"].f;
    float eps = 0;

    Vector2 playerCenter = Rect::GetCenter(position, size);
    Vector2 nextCenter = (nextTile + Vector2(0.5)) * Screen::tile_size;
    float h = Distance(playerCenter, nextCenter);

    if (InRange(nextTile, Vector2(), Vector2(15)) && visit.find(nextTile) == visit.end() && h <= maxDist)
    {
        int type = Map::Tiles[nextTile.y][nextTile.x].first;
        if (type & WALL)
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

            if (type & WALL)
                Screen::SetDrawColor(Color::white(Game::properties["ray_opacity"].i));
            else
                Screen::SetDrawColor(Color::red(Game::properties["ray_opacity"].i));
        }
        else if (type & COIN)
        {
            Screen::SetDrawColor(Color::yellow(Game::properties["ray_opacity"].i));

            if (Rect::IsColliding(playerCenter, Vector2(size.x / 6 * 4, size.y), nextCenter,
                                  Vector2(Screen::tile_size * 0.6f), 0))
            {
                current_score++;
                total_score++;

                Text::SetLabel("Play-0.score", "Score: " + str(total_score));

                print("player score", current_score, "/", win_score);

                if (Game::properties["sound"].b)
                    PlaySound("coin", CHANNEL_COIN, 0);

                if (current_score == win_score)
                {
                    Map::Tiles[Map::win_tile.y][Map::win_tile.x].first = WIN;
                    float wait = 500;
                    Map::AddTile(Map::win_tile.y, Map::win_tile.x, wait);
                }

                Tile *flying_coin = new Tile(Map::Tiles[nextTile.y][nextTile.x].second->position, Screen::tile_size);
                LinkedFunction *lf = new LinkedFunction(std::bind(
                    [](Tile *tile) {
                        Animate(tile, "coin");
                        return TransformValue(&tile->scale, Game::properties["tile_scale"].f * 0.6f,
                                              Game::properties["tile_rescale_speed"].f) &&
                               TransformVector2(&tile->position, Vector2(Screen::tile_size * 2, 0), 0.05f, 5);
                    },
                    flying_coin));
                lf->NextFunction(std::bind(
                    [](Tile *tile) {
                        Animate(tile, "coin");
                        return TransformValue<float>(&tile->scale, 0, Game::properties["tile_rescale_speed"].f);
                    },
                    flying_coin));
                lf->NextFunction(std::bind(
                    [](Tile *tile) {
                        delete tile;
                        tile = nullptr;
                        return 1;
                    },
                    flying_coin));
                lf->Execute();

                float wait = 0.1f;
                Map::RemoveTile(nextTile.y, nextTile.x, wait, false);
            }
        }
        else if (type & WIN)
        {
            Screen::SetDrawColor(Color::green(Game::properties["ray_opacity"].i));

            if (Rect::IsColliding(playerCenter, Vector2(size.x / 6 * 4, size.y), nextCenter, Vector2(Screen::tile_size),
                                  0) &&
                !won && Map::Tiles[Map::win_tile.y][Map::win_tile.x].second->scale == Game::properties["tile_scale"].f)
            {
                MapMaking::ToggleBtns(false);
                print("player won");
                won = true;
                print("deleting player...");
                LinkedFunction *lf = new LinkedFunction(
                    []() {
                        return TransformValue(&Game::player->scale, 0.0f, Game::properties["tile_rescale_speed"].f);
                    },
                    250);
                lf->NextFunction([]() {
                    PlaySound("win", CHANNEL_SPAWN_WIN, 0);
                    Game::time[1] = SDL_GetTicks();
                    delete Game::player;
                    Game::player = nullptr;
                    Map::current_map++;
                    MapMaking::ChangeMap();
                    return 1;
                });
                lf->NextFunction(
                    []() {
                        Scene::SpawnPlayer();
                        return 1;
                    },
                    Map::GetMapDelay(750));
                lf->Execute();
            }
        }
        else if (type & SPAWN)
        {
            Screen::SetDrawColor(Color::cyan(Game::properties["ray_opacity"].i));
        }

        if (type)
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

        velocity.d += Game::properties["gravity"].f;
        position.y += velocity.d;

        Background::Move(Vector2(0, -velocity.d), 0, 0.1);
        Background::Move(Vector2(0, -velocity.d), 1, 0.15);
        Background::Move(Vector2(0, -velocity.d), 2, 0.2);
    }
    else
    {
        if (current_state == JUMP)
        {
            current_state = previous_state;
            previous_state = JUMP;
        }

        if (velocity.d > Screen::resolution.x / 65.0f)
        {
            if (!Game::properties["player_immortal"].b)
            {
                if (Game::properties["sound"].b)
                    PlaySound("fall", CHANNEL_JUMP_FALL, 0);

                hp -= velocity.d / 2.5;
                Text::SetLabel("Play-1.hp", "Health: " + str(hp));

                Damaged(true);
                LinkedFunction *lf = new LinkedFunction(std::bind(
                                                            [](Player *player) {
                                                                if (player)
                                                                    player->Damaged(false);
                                                                return 1;
                                                            },
                                                            this),
                                                        500);
                lf->Execute();
            }
        }

        velocity.d = 0;
    }
}

void Player::DrawBox()
{
    Vector2 startPos(position.x + size.x / 6, position.y);
    Vector2 endPos(position.x + size.x / 6 * 5, position.y + size.y);

    Screen::SetDrawColor(Color::white(Game::properties["ray_opacity"].i));

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