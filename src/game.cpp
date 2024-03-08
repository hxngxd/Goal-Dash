#include "game.h"
#include "inputhandler.h"
#include "gameobject.h"

std::string Screen::title = "Game";
Vector2 Screen::resolution(768, 768);
int Screen::map_size = 16;
int Screen::player_size = Screen::resolution.x/Screen::map_size;
Vector2 Screen::bg_margin = Vector2(150);
Vector2 Screen::bg_cloud_position = -Screen::bg_margin;
Vector2 Screen::bg_star_position = -Screen::bg_margin;

float Game::fps = 60.0;
float Game::player_move_speed = 5;
float Game::player_acceleration = 0.04;
float Game::animation_speed = 15;
float Game::jump_speed = 10;
float Game::gravity = 0.3;
int Game::player_score = 0;
Vector2 Game::startingPosition = Vector2();

SDL_Event Game::event;
SDL_Window * Game::window = nullptr;
SDL_Renderer * Game::renderer = nullptr;

Player player1;

void Game::Update() {
    Screen::Clear(Color::black(255));
    Screen::Background();
    MapTile::Update();
    player1.Update();
    Screen::Display();
}

void Game::Start(){
    ShowMsg(1, normal, "trying to initializing SDL2...");
    if (!InitSDL2()){
        ShowMsg(1, fail, "init sdl2 failed.");
        return;
    }
    ShowMsg(1, success, "sdl2 ok!");

    ShowMsg(1, normal, "loading all medias...");
    if (!LoadMedia()){
        ShowMsg(1, fail, "failed to load medias.");
        return;
    }
    ShowMsg(1, success, "medias ok!");

    ShowMsg(1, normal, "creating map...");
    MapTile::Create();
    ShowMsg(2, success, "done.");

    ShowMsg(1, normal, "creating player...");
    player1.Init("Nguyen Tuong Hung", startingPosition);
    ShowMsg(2, success, "done.");

    running = true;
}

bool Game::InitSDL2(){
    ShowMsg(2, normal, "initializing SDL...");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        ShowMsg(3, fail, "sdl failed.");
        return 0;
    }
    else{
        ShowMsg(3, success, "done.");
    }

    ShowMsg(2, normal, "initializing SDL_Image...");
    if (!(IMG_Init(IMG_INIT_PNG))) {
        ShowMsg(3, fail, "sdl_image failed.");
        return 0;
    }
    else{
        ShowMsg(3, success, "done.");
    }

    ShowMsg(2, normal, "initializing SDL_TTF...");
    if (TTF_Init() != 0){
        ShowMsg(3, fail, "sdl_ttf failed.");
        return 0;
    }
    else{
        ShowMsg(3, success, "done.");
    }

    ShowMsg(2, normal, "creating window...");
    window = SDL_CreateWindow(Screen::title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen::resolution.x, Screen::resolution.y, 0);
    if (!window){
        ShowMsg(3, fail, "failed to create window.");
        return 0;
    }
    else{
        ShowMsg(3, success, "done.");
    }
    
    ShowMsg(2, normal, "creating renderer...");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer){
        ShowMsg(3, fail, "failed to create renderer.");
        return 0;
    }
    else{
        ShowMsg(3, success, "done.");
    }
    
    ShowMsg(2, normal, "turned on blend mode.");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    return 1;
}

bool Game::LoadMedia(){
    loadSprite("bg_star", "img/bg_star.png", 1, Vector2(4096));
    loadSprite("bg_cloud", "img/bg_cloud.png", 1, Vector2(4096));
    loadSprite("coin", "img/coin.png", 5, Vector2(16));
    loadSprite("idle", "img/idle.png", 10, Vector2(48));
    loadSprite("run", "img/run.png", 9, Vector2(48));
    loadSprite("jump", "img/jump.png", 4, Vector2(48));

    SDL_SetTextureBlendMode(Sprites["bg_cloud"]->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(Sprites["bg_star"]->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(Sprites["bg_cloud"]->texture, 150);
    return 1;
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

    ShowMsg(0, normal, "deleting all sprites...");
    for (auto & sprite : Sprites){
        std::string path = sprite.second->path;
        delete sprite.second;
        if (!sprite.second){
            ShowMsg(1, fail, "failed to delete " + path + ".");
        }
        else{
            ShowMsg(1, success, "deleted " + path + "!");
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

void Screen::SetDrawColor(SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Screen::Clear(SDL_Color color){
    SetDrawColor(color);
    SDL_RenderClear(renderer);
}

void Screen::Display(){
    SDL_RenderPresent(renderer);
}

void Screen::PointGrid(SDL_Color color){
    SetDrawColor(color);
    int sqr = Screen::resolution.x/Screen::map_size;
    for (int i=sqr;i<Screen::resolution.x;i+=sqr){
        for (int j=sqr;j<Screen::resolution.y;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

int Screen::bg_opacity = 64;
bool Screen::bg_toggle = true;

void Screen::Background(){
    DrawSprite(*Sprites["bg_cloud"], Screen::bg_cloud_position, resolution + Screen::bg_margin*2, 0, 0);

    if (bg_toggle){
        bg_opacity+=2; if (bg_opacity>=250) bg_toggle = false;
    }
    else{
        bg_opacity-=2; if (bg_opacity<=64) bg_toggle = true;
    }
    SDL_SetTextureAlphaMod(Sprites["bg_star"]->texture, bg_opacity);
    DrawSprite(*Sprites["bg_star"], Screen::bg_star_position, resolution + Screen::bg_margin*2, 0, 0);
}

void Screen::DrawSprite(Sprite & sprite, Vector2 position, Vector2 size, int currentFrame, bool flip){
    SDL_Rect src = {(currentFrame%sprite.maxFrames)*sprite.realSize.x, 0, sprite.realSize.x, sprite.realSize.y};
    SDL_Rect dst = {position.x, position.y, size.x, size.y};
    SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}

void ShowMsg(int indent, msg_types type, std::string msg){
    std::cout << std::string(indent*2, ' ');
    switch (type){
        case normal:
            std::cout << "> ";
            break;
        case success:
            std::cout << "$ ";
            break;
        case fail:
            std::cout << "@ ";
            break;
        case error:
            std::cout << "! ";
            break;
        case logging:
            std::cout << "# ";
            break;
        default:
            break;
    }
    std::cout << msg << std::endl;
}