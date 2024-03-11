#include "game.h"
#include "gameobject.h"

std::string Screen::title = "hxngxd";
Vector2 Screen::resolution(768, 768);
int Screen::map_size = 16;
int Screen::tile_size = 48;

float Game::fps = 60.0;
float Game::gravity = 0.3;

SDL_Event Game::event;
SDL_Window * Game::window = nullptr;
SDL_Renderer * Game::renderer = nullptr;

Player player1;

void Game::Update() {
    Screen::Clear(Color::black(255));
    Background::Draw();
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

    ShowMsg(1, normal, "playing background music...");
    playMusic("bg_music", -1);
    Mix_VolumeMusic(32);
    ShowMsg(2, success, "done.");

    ShowMsg(1, normal, "creating map...");
    auto m = MapTile::CreateTiles();
    ShowMsg(2, success, "done.");

    ShowMsg(1, normal, "creating player 1...");
    player1.wait_for_animation = m.first;
    player1.starting_position = Vector2(m.second.y * Screen::tile_size, m.second.x * Screen::tile_size);
    player1.Init("Nguyen Tuong Hung");
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

    ShowMsg(2, normal, "initializing SDL_Mixer format...");
    int format = MIX_INIT_OGG;
    if (Mix_Init(format) & format != format){
        ShowMsg(3, fail, "sdl_mixer format failed.");
        return 0;
    }
    else{
        ShowMsg(3, success, "done.");
    }

    ShowMsg(2, normal, "initializing SDL_Mixer audio device...");
    if (Mix_OpenAudio(44100, AUDIO_S32SYS, 2, 4096) < 0){
        ShowMsg(3, fail, "sdl_mixer audio device failed.");
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
    if (!loadSprite("coin", "img/coin.png", 5, Vector2(16))) return 0;
    if (!loadSprite("idle", "img/idle.png", 10, Vector2(48))) return 0;
    if (!loadSprite("run", "img/run.png", 9, Vector2(48))) return 0;
    if (!loadSprite("jump", "img/jump.png", 4, Vector2(48))) return 0;

    if (!Background::loadBackground("bg_star", "img/bg_star.png", 1, Vector2(4096), 2)) return 0;
    if (!Background::loadBackground("bg_star1", "img/bg_star.png", 1, Vector2(4096), 1.5)) return 0;
    if (!Background::loadBackground("bg_cloud", "img/bg_cloud.png", 1, Vector2(4096), 1.25)) return 0;

    if (!loadSound("coin", "sound/coin.ogg")) return 0;
    if (!loadSound("jump", "sound/jump.ogg")) return 0;
    if (!loadSound("run", "sound/run.ogg")) return 0;
    if (!loadSound("fall", "sound/fall.ogg")) return 0;
    if (!loadMusic("bg_music", "sound/bg_music.ogg")) return 0;

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
        KeyboardHandler::PlayerInputHandler(player1, leftKeys);
    }
}

void Game::Quit(){
    ShowMsg(0, normal, "deleting all sprites...");
    for (auto & sprite : Sprites){
        std::string path = sprite.second->path;
        delete sprite.second;
        sprite.second = nullptr;
        if (sprite.second){
            ShowMsg(1, fail, "failed to delete " + path + ".");
        }
        else{
            ShowMsg(1, success, "deleted " + path + "!");
        }
    }

    ShowMsg(0, normal, "deleting all sounds and musics...");
    for (auto & sound : Sounds){
        Mix_FreeChunk(sound.second);
        sound.second = nullptr;
        if (sound.second){
            ShowMsg(1, fail, "failed to delete " + sound.first + ".");
        }
        else{
            ShowMsg(1, success, "deleted " + sound.first + "!");
        }
    }
    for (auto & music : Musics){
        Mix_FreeMusic(music.second);
        music.second = nullptr;
        if (music.second){
            ShowMsg(1, fail, "failed to delete " + music.first + ".");
        }
        else{
            ShowMsg(1, success, "deleted " + music.first + "!");
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
}

void Screen::SetDrawColor(
    SDL_Color color
){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Screen::Clear(
    SDL_Color color
){
    SetDrawColor(color);
    SDL_RenderClear(renderer);
}

void Screen::Display(){
    SDL_RenderPresent(renderer);
}

void Screen::PointGrid(
    SDL_Color color
){
    SetDrawColor(color);
    int sqr = Screen::resolution.x/Screen::map_size;
    for (int i=sqr;i<Screen::resolution.x;i+=sqr){
        for (int j=sqr;j<Screen::resolution.y;j+=sqr){
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void Screen::DrawSprite(
    Sprite & sprite,
    const Vector2 & position,
    const Vector2 & size,
    float scale,
    int currentFrame,
    bool flip)
{
    SDL_Rect src = {(currentFrame % sprite.maxFrames) * sprite.realSize.x, 0, sprite.realSize.x, sprite.realSize.y};
    SDL_Rect dst = Rect::reScale(position, size, scale);
    SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}

void ShowMsg(
    int indent,
    msg_types type,
    std::string msg
){
    std::cout << std::string(indent * 2, ' ');
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