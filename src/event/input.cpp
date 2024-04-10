#include "input.h"
#include "../datalib/mixer.h"
#include "../game.h"
#include "ui.h"

Vector2 EventHandler::MousePosition;
bool EventHandler::isMouseLeft = false;
std::map<std::string, std::function<void()>> EventHandler::MouseDownActions;
std::map<std::string, std::function<void()>> EventHandler::MouseUpActions;

void EventHandler::PlayerInputHandler(Player *player, Keys &keys)
{
    if (Game::event.type == SDL_KEYDOWN)
    {
        auto key = Game::event.key.keysym.sym;
        if (key == keys.right)
            player->key_right = true;
        else if (key == keys.left)
            player->key_left = true;
        else if (key == keys.down)
            player->key_down = true;
        else if (key == keys.up)
            player->key_up = true;
        else if (key == keys.space)
        {
            if (player->collide_down.second && !player->collide_up.second && !Game::Properties["no_gravity"].b)
            {
                player->velocity.d = -Game::Properties["player_jump_speed"].f;
                player->position.y += player->velocity.d;
                player->previous_state = player->current_state;
                player->current_state = JUMP;
                if (Game::Properties["sound"].b)
                    PlaySound("jump", CHANNEL_JUMP_FALL, 0);
                player->collide_down.second = false;
            }
        }
    }
    else if (Game::event.type == SDL_KEYUP)
    {
        auto key = Game::event.key.keysym.sym;
        if (key == keys.right)
            player->key_right = false;
        else if (key == keys.left)
            player->key_left = false;
        else if (key == keys.down)
            player->key_down = false;
        else if (key == keys.up)
            player->key_up = false;
    }
}

void EventHandler::MouseInputHandler()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    MousePosition = Vector2(x, y);

    if (Game::event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (Game::event.button.button == SDL_BUTTON_LEFT)
        {
            for (auto &action : MouseDownActions)
                action.second();
        }
    }
    else if (Game::event.type == SDL_MOUSEBUTTONUP)
    {
        if (Game::event.button.button == SDL_BUTTON_LEFT)
        {
            for (auto &action : MouseUpActions)
                action.second();
        }
    }
}

void EventHandler::Update()
{
    while (SDL_PollEvent(&Game::event) != 0)
    {
        if (Game::event.type == SDL_QUIT)
            game->running = false;

        EventHandler::MouseInputHandler();

        if (Game::player)
            EventHandler::PlayerInputHandler(Game::player,
                                             Game::Properties["keyboard_layout"].b ? right_keys : left_keys);
    }
}