#include "game.h"
#include "inputhandler.h"

const char * Screen::title = "Game";
Vector2 Screen::resolution(768, 768);
int Screen::map_size = 16;
int Screen::player_size = Screen::resolution.x/Screen::map_size;

float Game::fps = 60.0;
float Game::player_moving_speed = 5;
float Game::player_acceleration_rate = 0.04;
float Game::animation_speed = 15;
float Game::jump_speed = 9;
float Game::gravity = 0.4;

SDL_Event Game::event;
SDL_Window * Game::window = nullptr;
SDL_Renderer * Game::renderer = nullptr;

std::vector<std::vector<int>> Game::map = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

Player player1;

void Game::Update() {
    Renderer::Clear(Color::black(255));
    Renderer::PointGrid(Color::white(127));
    MapTile::Update();
    player1.Update();
    Renderer::Display();
}

void Game::Start(){
    Init();
    if (!running) return;

    Sprite("img/wall.png", 1, Vector2(1024));
    Sprite("img/coin.png", 5, Vector2(16));
    Sprite("img/idle.png", 10, Vector2(48));
    Sprite("img/run.png", 9, Vector2(48));
    Sprite("img/jump.png", 4, Vector2(48));

    MapTile::Create(Game::map);

    player1 = Player("Nguyen Tuong Hung", Vector2(100));
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
    if (!Screen::Init()) return;
    if (!Renderer::Init()) return;
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
        KeyboardHandler::LeftPlayerInputHandler(player1);
    }
}

void Game::Quit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Screen::Init() {
    window = SDL_CreateWindow(title, x, y, resolution.x, resolution.y, fixedsize);
    if (window==nullptr){
        std::cout << "Error: Failed to open window - " << SDL_GetError();
        return 0;
    }
    else{
        std::cout << "Window created!" << std::endl;
        return 1;
    }
}

bool Renderer::Init(){
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer==nullptr){
        std::cout << "Error: Failed to create renderer - " << SDL_GetError();
        return 0;
    }
    else{
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        std::cout << "Renderer created!" << std::endl;
        return 1;
    }
}

void Renderer::SetDrawColor(SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Renderer::Clear(SDL_Color color){
    SetDrawColor(color);
    SDL_RenderClear(renderer);
}

void Renderer::Display(){
    SDL_RenderPresent(renderer);
}

void Renderer::PointGrid(SDL_Color color){
    SetDrawColor(color);
    int sqr = Screen::resolution.x/Screen::map_size;
    for (int i=sqr;i<Screen::resolution.x;i+=sqr){
        for (int j=sqr;j<Screen::resolution.y;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void Renderer::DrawSprite(Sprite & sprite, Vector2 position, Vector2 size, int currentFrame, bool flip){
    SDL_Rect src = {(currentFrame%sprite.maxFrames)*sprite.realSize.x, 0, sprite.realSize.x, sprite.realSize.y};
    SDL_Rect dst = {position.x, position.y, size.x, size.y};
    SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}