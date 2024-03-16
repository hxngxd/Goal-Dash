#include "game.h"
#include "gameobject.h"

Vector2 Screen::resolution(768, 768);
int Screen::map_size = 16;
int Screen::tile_size = 48;

SDL_Event Game::event;
SDL_Window *Game::window = nullptr;
SDL_Renderer *Game::renderer = nullptr;

std::map<float, DelayFunction *> DelayFunctions;
std::map<std::string, PropertiesType> Game::Properties;

void Game::Update()
{
    //----------------------------------------

    Screen::Clear(Color::black(255));

    if (Game::Properties["point_grid"].b)
        Screen::PointGrid(Color::white(255));

    if (Game::Properties["background"].b)
        Background::Draw();

    //----------------------------------------

    DelayFunction::Update();

    //----------------------------------------

    Screen::Display();
}

void Game::Start()
{
    //----------------------------------------

    ShowMsg(1, normal, "loading config file...");
    if (!LoadConfig())
    {
        ShowMsg(1, fail, "load config failed.");
        return;
    }
    ShowMsg(1, success, "config loaded!");

    //----------------------------------------

    ShowMsg(1, normal, "trying to initializing SDL2...");
    if (!InitSDL2())
    {
        ShowMsg(1, fail, "init sdl2 failed.");
        return;
    }
    ShowMsg(1, success, "sdl2 ok!");

    //----------------------------------------

    ShowMsg(1, normal, "loading all medias...");
    if (!LoadMedia())
    {
        ShowMsg(1, fail, "failed to load medias.");
        return;
    }
    ShowMsg(1, success, "medias ok!");

    //----------------------------------------

    if (Game::Properties["music"].b)
    {
        ShowMsg(1, normal, "playing background music...");
        playMusic("bg_music", -1);
        Mix_VolumeMusic(Game::Properties["music_volume"].i);
        ShowMsg(2, success, "done.");
    }

    //----------------------------------------

    ShowMsg(1, normal, "creating border...");
    MapTile::CreateBorder();
    ShowMsg(2, success, "done.");

    //----------------------------------------

    ShowMsg(1, normal, "loading font...");
    myFont = TTF_OpenFont(Game::Properties["font"].s.c_str(), Game::Properties["font_size"].i);
    if (!myFont)
    {
        ShowMsg(1, fail, "failed to open font.");
        return;
    }
    ShowMsg(2, success, "done.");

    //----------------------------------------

    // ShowMsg(1, normal, "create welcome scene.");
    // ShowMsg(1, normal, "creating map...");
    // auto m = MapTile::CreateTiles(Game::Properties["map"].s);
    // ShowMsg(2, success, "done.");

    // ShowMsg(1, normal, "creating player 1: " + Game::Properties["player_name"].s + "...");
    // player1.starting_position = Vector2(m.second.y * Screen::tile_size, m.second.x * Screen::tile_size);
    // player1.Init(Game::Properties["player_name"].s);
    // GameObject::reScale(&player1, 1, m.first, Game::Properties["rescale_speed"].f, [](){
    //     Game::Properties["playable"].b = true;
    // });
    // ShowMsg(2, success, "done.");

    running = true;
}

bool Game::InitSDL2()
{
    //----------------------------------------

    ShowMsg(2, normal, "initializing SDL...");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        ShowMsg(3, fail, "sdl failed.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "initializing SDL_Image...");
    if (!(IMG_Init(IMG_INIT_PNG)))
    {
        ShowMsg(3, fail, "sdl_image failed.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "initializing SDL_TTF...");
    if (TTF_Init() != 0)
    {
        ShowMsg(3, fail, "sdl_ttf failed.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "initializing SDL_Mixer format...");
    int format = MIX_INIT_OGG;
    if (Mix_Init(format) & format != format)
    {
        ShowMsg(3, fail, "sdl_mixer format failed.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "initializing SDL_Mixer audio device...");
    if (Mix_OpenAudio(44100, AUDIO_S32SYS, 2, 4096) < 0)
    {
        ShowMsg(3, fail, "sdl_mixer audio device failed.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "creating window...");
    window = SDL_CreateWindow("Goal Dash", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen::resolution.x, Screen::resolution.y, 0);
    if (!window)
    {
        ShowMsg(3, fail, "failed to create window.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "creating renderer...");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        ShowMsg(3, fail, "failed to create renderer.");
        return 0;
    }
    else
        ShowMsg(3, success, "done.");

    //----------------------------------------

    ShowMsg(2, normal, "turned on blend mode.");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //----------------------------------------

    return 1;
}

bool Game::LoadMedia()
{
    //----------------------------------------

    if (!loadSprite("coin", "img/coin.png", 5, Vector2(16)))
        return 0;
    if (!loadSprite("idle", "img/idle.png", 10, Vector2(48)))
        return 0;
    if (!loadSprite("run", "img/run.png", 9, Vector2(48)))
        return 0;
    if (!loadSprite("jump", "img/jump.png", 4, Vector2(48)))
        return 0;

    //----------------------------------------

    if (!Background::loadBackground("bg_cloud", "img/bg_cloud.png", 1, Vector2(4096), 1.25))
        return 0;
    if (!Background::loadBackground("bg_star1", "img/bg_star.png", 1, Vector2(4096), 1.5))
        return 0;
    if (!Background::loadBackground("bg_star", "img/bg_star.png", 1, Vector2(4096), 2))
        return 0;

    //----------------------------------------

    if (!loadSound("coin", "sound/coin.ogg"))
        return 0;
    if (!loadSound("jump", "sound/jump.ogg"))
        return 0;
    if (!loadSound("run", "sound/run.ogg"))
        return 0;
    if (!loadSound("fall", "sound/fall.ogg"))
        return 0;
    if (!loadMusic("bg_music", "sound/bg_music.ogg"))
        return 0;

    //----------------------------------------

    return 1;
}

void Game::HandleEvent()
{
    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
        // KeyboardHandler::PlayerInputHandler(player1, Game::Properties["keyboard_layout"].b ? rightKeys : leftKeys);
    }
}

void Game::Quit()
{
    //----------------------------------------

    ShowMsg(0, normal, "deleting all tiles...");
    for (int i = 0; i < Screen::map_size; i++)
    {
        for (int j = 0; j < Screen::map_size; j++)
        {
            if (!TileMap[i][j].second)
                continue;
            delete TileMap[i][j].second;
            TileMap[i][j].second = nullptr;
        }
    }
    ShowMsg(1, success, "done.");

    //----------------------------------------

    ShowMsg(0, normal, "deleting all sprites...");
    for (auto &sprite : Sprites)
    {
        std::string path = sprite.second->path;
        delete sprite.second;
        sprite.second = nullptr;
        if (sprite.second)
            ShowMsg(1, fail, "failed to delete " + path + ".");
        else
            ShowMsg(1, success, "deleted " + path + "!");
    }
    ShowMsg(1, success, "done.");

    //----------------------------------------

    ShowMsg(0, normal, "deleting all sounds and musics...");
    for (auto &sound : Sounds)
    {
        Mix_FreeChunk(sound.second);
        sound.second = nullptr;
        if (sound.second)
            ShowMsg(1, fail, "failed to delete " + sound.first + ".");
        else
            ShowMsg(1, success, "deleted " + sound.first + "!");
    }
    ShowMsg(1, success, "done.");

    for (auto &music : Musics)
    {
        Mix_FreeMusic(music.second);
        music.second = nullptr;
        if (music.second)
            ShowMsg(1, fail, "failed to delete " + music.first + ".");
        else
            ShowMsg(1, success, "deleted " + music.first + "!");
    }

    ShowMsg(1, success, "done.");

    //----------------------------------------

    ShowMsg(0, normal, "deleting font...");
    TTF_CloseFont(myFont);
    ShowMsg(1, success, "done.");

    //----------------------------------------

    ShowMsg(0, normal, "closing sdl2...");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    ShowMsg(1, success, "done.");

    //----------------------------------------
}

void Screen::SetDrawColor(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Screen::Clear(SDL_Color color)
{
    SetDrawColor(color);
    SDL_RenderClear(renderer);
}

void Screen::Display()
{
    SDL_RenderPresent(renderer);
}

void Screen::PointGrid(SDL_Color color)
{
    SetDrawColor(color);
    int sqr = resolution.x / map_size;
    for (int i = sqr; i < resolution.x; i += sqr)
    {
        for (int j = sqr; j < resolution.y; j += sqr)
        {
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void Screen::DrawSprite(Sprite &sprite, const Vector2 &position, const Vector2 &size, float scale, int currentFrame, bool flip)
{
    SDL_Rect src = {(currentFrame % sprite.maxFrames) * sprite.realSize.x, 0, sprite.realSize.x, sprite.realSize.y};
    SDL_Rect dst = Rect::reScale(position, size, scale);
    SDL_RenderCopyEx(renderer, sprite.texture, &src, &dst, 0, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
}