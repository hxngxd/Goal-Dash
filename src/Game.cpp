#include "game.h"
#include "screen.h"
#include "render.h"
#include "keyboard.h"

const char * title = "Fun game";
const Vector2 resolution(768, 768);
const int map_size = 16;
const int player_size = resolution.x/map_size;

float fps = 60.0;
float player_speed = 4;
float player_acceleration_rate = 0.05;
float animation_speed = 20;
float falling_speed = 0.5;
float gravity = 5;

std::vector<std::vector<int>> tileMap;

Map newMap(map_size, map_size, {0.99, 0.01}, tileMap);

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

    player1.Init("Hoang le minh", Vector2(150, 150));
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
    while (SDL_PollEvent(&event) != 0){
        if (event.type == SDL_QUIT){
            running = true;
        }
        KeyboardHandler::PlayerMovement(player1);
    }
}

void Game::Update() {
    Renderer::Clear(Color::black(255));
    Renderer::PointGrid(Color::white(127));
    player1.Update();
    player1.Animation();
    newMap.Draw(tileMap);
    Renderer::Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}