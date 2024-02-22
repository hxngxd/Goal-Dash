#include "game.h"
#include "screen.h"
#include "render.h"

const char * title = "Fun game";
const int width = 800;
const int height = 800;
const int map_size = 16;

float fps = 60.0;
float speed = 7;

Direction velocity;

SDL_Window * window = nullptr;
SDL_Renderer * renderer = nullptr;

SDL_Event Game::event;

SDL_Texture * player = nullptr;
Vector2 playerPos;

void Game::Start(){
    Init();
    if (!running) return;

    player = TextureManager::LoadTexture("player", "texture/board.png");
}

void Game::Init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "Error: SDL_IMAGE failed to initialize - " << SDL_GetError();
        return;
    }

    if (!Screen::CreateWindow()) return;

    if (!Renderer::CreateRenderer()) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    running = true;
}

bool Game::isRunning(){
    return running;
}

void Game::HandleEvent(){
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void Game::Update() {
    if (event.type==SDL_KEYDOWN){
        switch (event.key.keysym.sym){
            case SDLK_w:
                velocity.u = -1;
                break;
            case SDLK_a:
                velocity.l = -1;
                break;
            case SDLK_s:
                velocity.d = 1;
                break;
            case SDLK_d:
                velocity.r = 1;
                break;
            default:
                break;
        }
    }
    if (event.type==SDL_KEYUP){
        switch (event.key.keysym.sym){
            case SDLK_w:
                velocity.u = 0;
                break;
            case SDLK_a:
                velocity.l = 0;
                break;
            case SDLK_s:
                velocity.d = 0;
                break;
            case SDLK_d:
                velocity.r = 0;
                break;
            default:
                break;
        }
    }
    playerPos += Vector2((velocity.l + velocity.r) * speed, (velocity.u + velocity.d) * speed);
}

void Game::Render(){
    Renderer::Clear(Color::black(255));
    Renderer::PointGrid(Color::white(127));
    SDL_Rect src = Rect::Square(256);
    SDL_Rect dst = {playerPos.x, playerPos.y, 50, 50};
    SDL_RenderCopy(renderer, player, &src, &dst);
    Renderer::Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}