#include "game.h"

class Screen {
public:
    static const int x = SDL_WINDOWPOS_CENTERED;
    static const int y = SDL_WINDOWPOS_CENTERED;

    static const Uint32 fixedsize = 0;
    static const Uint32 resizable = SDL_WINDOW_RESIZABLE;

    static bool CreateWindow();
};