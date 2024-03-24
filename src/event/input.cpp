#include "input.h"
#include "../datalib/mixer.h"
#include "../game.h"
#include "ui.h"

Vector2 mousePosition = Vector2();

void EventHandler::PlayerInputHandler(Player *player, Keys &keys)
{
    if (Game::event.type == SDL_KEYDOWN)
    {
        auto key = Game::event.key.keysym.sym;
        if (key == keys.right)
        {
            player->key_right = true;
        }
        else if (key == keys.left)
        {
            player->key_left = true;
        }
        else if (key == keys.down)
        {
            player->key_down = true;
        }
        else if (key == keys.up)
        {
            player->key_up = true;
        }
        else if (key == keys.space)
        {
            if (player->collide_down.second && !Game::Properties["no_gravity"].b)
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
        {
            player->key_right = false;
        }
        else if (key == keys.left)
        {
            player->key_left = false;
        }
        else if (key == keys.down)
        {
            player->key_down = false;
        }
        else if (key == keys.up)
        {
            player->key_up = false;
        }
        else if (key == keys.space)
        {
        }
    }
}

void EventHandler::MouseInputHandler()
{
    if (Game::event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (Game::event.button.button == SDL_BUTTON_LEFT)
        {
            downButton = hoverButton;
        }
    }
    else if (Game::event.type == SDL_MOUSEBUTTONUP)
    {
        if (Game::event.button.button == SDL_BUTTON_LEFT)
        {
            upButton = hoverButton;
            if (downButton == upButton && Buttons.find(downButton) != Buttons.end() && Buttons[downButton])
            {
                if (SDL_GetTicks() - lastClicked >= 500)
                {
                    Buttons[downButton]->onClick();
                    lastClicked = SDL_GetTicks();
                }
            }
        }
    }
}