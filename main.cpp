#include <bits/stdc++.h>
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_image.h"
#include "include/SDL2/SDL_mixer.h"
#include "include/SDL2/SDL_ttf.h"
#include "mylib/Screen.hpp"
#include "mylib/Render.hpp"
#include "mylib/Entity.hpp"

int main(int argc, char* argv[]){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return 1;
    }

    Screen currentScreen;
    Render currentRenderer(currentScreen.window);

    SDL_Texture * bg_texture = currentRenderer.loadTexture("texture/board.png");
    Entity background({0, 0}, {0, 0, 256, 256}, bg_texture);
    
    // std::vector<Entity> entities = {
    //     Entity({0, 0}, {0, 0, 256, 256}, bg_texture),
    //     Entity({255, 255}, {0, 0, 256, 256}, bg_texture)
    // };

    bool running = true;
    SDL_Event event;

    while(running){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        currentRenderer.Clear();
        currentRenderer.RenderIt(background);
        currentRenderer.Display();
    }

    currentScreen.CleanUp();
    SDL_Quit();
    return 0;
}