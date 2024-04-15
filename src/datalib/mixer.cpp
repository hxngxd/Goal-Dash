#include "mixer.h"

std::map<std::string, Mix_Chunk *> Sounds;
Mix_Music *Music = nullptr;

bool LoadSound(std::string name, std::string path)
{
    print("loading sound:", path);
    Sounds[name] = Mix_LoadWAV(path.c_str());
    if (!Sounds[name])
    {
        Mix_FreeChunk(Sounds[name]);
        Sounds.erase(name);
        print("failed to load sound:", path);
        return 0;
    }
    print(path, "ok");
    return 1;
}

void PlaySound(std::string name, int channel, int loop)
{
    Mix_PlayChannel(channel, Sounds[name], loop);
}

void StopSound(int channel)
{
    Mix_FadeOutChannel(channel, 50);
}

void StopAllSound()
{
    StopSound(-1);
}

bool LoadMusic(std::string path)
{
    print("loading music:", path);
    Music = Mix_LoadMUS(path.c_str());
    if (!Music)
    {
        Mix_FreeMusic(Music);
        print("failed to load music:", path);
        return 0;
    }
    print(path, "ok");
    return 1;
}

void PlayMusic(int loop)
{
    Mix_PlayMusic(Music, loop);
}

void StopMusic()
{
    Mix_HaltMusic();
}