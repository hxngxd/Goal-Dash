#pragma once
#include "../../include/SDL2/SDL_mixer.h"
#include "msg.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

extern std::map<std::string, Mix_Chunk *> Sounds;
extern Mix_Music *Music;

bool LoadSound(std::string name, std::string path);

void PlaySound(std::string name, int channel, int loop);

void StopSound(int channel);

void StopAllSound();

bool LoadMusic(std::string path);

void PlayMusic(int loop);

void StopMusic();

enum mixer_channels
{
    CHANNEL_JUMP_FALL = 0,
    CHANNEL_RUN_BUILD,
    CHANNEL_BUTTON_HOVER,
    CHANNEL_BUTTON_CLICK,
    CHANNEL_COIN,
    CHANNEL_HEALTH,
    CHANNEL_SPAWN_WIN_DIE,
};