#include "game.h"

bool Sound::Init(){
    int format = MIX_INIT_MP3 | MIX_INIT_WAVPACK;
    int format_init = Mix_Init(format);
    if (format_init & format != format){
        std::cout << "Error: Audio format could not be initialize! - " << Mix_GetError();
        return 0;
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_F32SYS, 2, 4096) < 0){
        std::cout << "Error: SDL_Mixer could not be initialize! - " << Mix_GetError();
        return 0;
    }
    return 1;
}