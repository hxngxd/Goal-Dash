#include "game.h"
#include "inputhandler.h"

std::string Screen::title = "Game";
Vector2 Screen::resolution(768, 768);
int Screen::map_size = 16;
int Screen::player_size = Screen::resolution.x/Screen::map_size;

float Game::fps = 60.0;
float Game::player_moving_speed = 5;
float Game::player_acceleration_rate = 0.04;
float Game::animation_speed = 15;
float Game::jump_speed = 10;
float Game::gravity = 0.3;
int Game::view_mode = 1;
int Game::player_score = 0;
Vector2 Game::startingPosition = Vector2();

SDL_Event Game::event;
SDL_Window * Game::window = nullptr;
SDL_Renderer * Game::renderer = nullptr;

std::vector<std::vector<int>> Game::map = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 3, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 3, 1},
    {1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, -2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

AllSprite Sprites;

Player player1;

void Game::Update() {
    Renderer::Clear(Color::black(255));
    Renderer::PointGrid(Color::white(127));
    MapTile::Update();
    player1.Update();
    Screen::DisplayText(std::to_string(Game::player_score));
    Renderer::Display();
}

void Game::Start(){
    Init();
    if (!running) return;

    Sprites.LoadAllSprite();

    MapTile::Create(Game::map);

    player1 = Player("Nguyen Tuong Hung", Game::startingPosition);
}

void Game::Init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error: SDL failed to initialize - " << SDL_GetError();
        return;
    }
    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "Error: SDL_IMAGE failed to initialize - " << IMG_GetError();
        return;
    }
    if (TTF_Init() != 0){
        std::cout << "Error: SDL_TTF failed to initialize - " << TTF_GetError();
        return;
    }
    if (!Screen::Init()) return;
    if (!Renderer::Init()) return;
    if (!Sound::Init()) return;
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
    TTF_Quit();
    Mix_Quit();
}

bool Screen::Init() {
    window = SDL_CreateWindow(title.c_str(), x, y, resolution.x, resolution.y, fixedsize);
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

void Screen::DisplayText(std::string text){
    TTF_Font * font = TTF_OpenFont("fonts/Roboto-Bold.ttf", 24);
    SDL_Surface * surface = TTF_RenderText_Solid(font, text.c_str(), Color::white(255));

    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = rect.y = 100;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}