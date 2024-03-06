#include "game.h"

bool Sound::Init(){
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        std::cout << "Error: SDL_Mixer could not be initialize! - " << Mix_GetError();
        return 0;
    }
    return 1;
}