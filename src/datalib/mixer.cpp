#include "mixer.h"

std::map<std::string, Mix_Chunk *> Sounds;
std::map<std::string, Mix_Music *> Musics;

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

bool LoadMusic(std::string name, std::string path)
{
    print("loading music:", path);
    Musics[name] = Mix_LoadMUS(path.c_str());
    if (!Musics[name])
    {
        Mix_FreeMusic(Musics[name]);
        Musics.erase(name);
        print("failed to load music:", path);
        return 0;
    }
    print(path, "ok");
    return 1;
}

void PlayMusic(std::string name, int loop)
{
    Mix_PlayMusic(Musics[name], loop);
}

void StopMusic()
{
    Mix_HaltMusic();
}