#include "game.h"
#include "screen.h"
#include "render.h"

const char * title = "Fun game";
const Vector2 resolution(800, 800);
const int map_size = 32;
const int player_size = 179;

float fps = 60.0;
float player_speed = 5;
float animation_speed = 20;
float falling_speed = 0.5;
float gravity = 10;

SDL_Window * window = nullptr;
SDL_Renderer * renderer = nullptr;

SDL_Event Game::event;

Sprite sprite_idle;
Sprite sprite_run;

Player player1;
void Game::Start(){
    Init();
    if (!running) return;

    sprite_idle.LoadSprite("img/idle.png", 10, Vector2(48, 48));
    sprite_run.LoadSprite("img/run.png", 9, Vector2(48, 48));

    player1.Init("Hoang le minh", Vector2());
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
    Renderer::Clear(Color::black(255));
    Renderer::PointGrid(Color::white(127));
    player1.Update();
    player1.Animation();
    Renderer::Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}