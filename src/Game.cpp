#include "game.h"

const char * title = "Game";
const Vector2 resolution(768, 768);
const int map_size = 16;
const int player_size = resolution.x/map_size;

float fps = 60.0;
float player_moving_speed = 5;
float player_acceleration_rate = 0.05;
float animation_speed = 20;
float jump_speed = 9;
float gravity = 0.4;

std::vector<std::vector<int>> map = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 0, 1, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

SDL_Window * window = nullptr;
SDL_Renderer * renderer = nullptr;

SDL_Event Game::event;

Player player1;

Sprite MapTile::tileSprite_1;

void Game::Start(){
    Init();
    if (!running) return;

    MapTile::Create(map);
    player1.Init("hxngxd", Vector2(100, 100));
    MapTile::tileSprite_1.LoadSprite("img/wall.png", 1, Vector2(1024));
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
            running = false;
        }
        KeyboardHandler::InputHandler();
    }
}

void Game::Update() {
    Renderer::Clear(Color::black(255));
    // Renderer::PointGrid(Color::white(127));
    MapTile::Draw();
    player1.Update();
    Renderer::Display();
}

void Game::Quit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}