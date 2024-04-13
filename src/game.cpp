#include "game.h"
#include "datalib/mixer.h"
#include "datalib/msg.h"
#include "datalib/sprite.h"
#include "datalib/util.h"
#include "event/input.h"
#include "event/scene.h"
#include "event/ui.h"
#include "func/func.h"

bool Game::running = false;
std::map<std::string, PropertiesType> Game::properties;
SDL_Event Game::event;
SDL_Window *Game::window = nullptr;
SDL_Renderer *Game::renderer = nullptr;
Player *Game::player = nullptr;
int Game::time[3] = {0, 0, 0};

Vector2 Screen::resolution;
int Screen::map_size;
int Screen::tile_size;
std::vector<int> Screen::resolutions = {320, 480, 640, 800, 960, 1120};
int Screen::current_resolution;

void Game::Update()
{
    //----------------------------------------

    Screen::Clear(Color::black());

    if (properties["point_grid"].b)
        Screen::PointGrid(Color::white());

    if (properties["background_enable"].b)
        Background::Update();

    //----------------------------------------

    Map::Update();

    if (Map::mode == 1)
        MapMaking::Update();

    EventHandler::Update();

    if (player)
    {
        player->Update();
        Text::SetLabel("Play-0.time", "Time: " + FormatMS(SDL_GetTicks() - time[0]));
    }

    UI::Update();

    LinkedFunction::Update();

    UI::RemoveUIs();
    //----------------------------------------

    Screen::Display();
}

void Game::Start()
{
    //----------------------------------------

    print("loading config file...");
    if (!LoadConfig())
    {
        print("failed to load config file");
        return;
    }
    print("config file ok");

    //----------------------------------------

    print("initializing sdl2 libraries...");
    if (!InitSDL2())
    {
        print("failed to initialize sdl2");
        return;
    }
    print("sdl2 ok");

    //----------------------------------------

    print("loading medias...");
    if (!LoadMedia())
    {
        print("failed to load medias");
        return;
    }
    print("medias ok");

    //----------------------------------------

    if (properties["music"].b)
    {
        print("playing background music...");
        PlayMusic("bg_music", -1);
        Mix_VolumeMusic(properties["volume"].i);
    }

    //----------------------------------------

    print("loading font...");
    myFont = TTF_OpenFont("fonts/myfont.ttf", 24);
    if (!myFont)
    {
        print("failed to load font");
        return;
    }
    print("font loaded");

    //----------------------------------------

    UI::Start();
    Scene::Welcome();
    Scene::Common();
    Scene::Settings();

    //----------------------------------------

    running = true;
}

bool Game::InitSDL2()
{
    //----------------------------------------

    print("initializing sdl...");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        print("sdl failed");
        return 0;
    }
    else
        print("sdl ok");

    //----------------------------------------

    print("initializing sdl_image...");
    if (!(IMG_Init(IMG_INIT_PNG)))
    {
        print("sdl_image failed");
        return 0;
    }
    else
        print("sdl_image ok");

    //----------------------------------------

    print("initializing sdl_ttf...");
    if (TTF_Init() != 0)
    {
        print("sdl_ttf failed");
        return 0;
    }
    else
        print("sdl_ttf ok");

    //----------------------------------------

    print("initializing sdl_mixer...");
    int format = MIX_INIT_OGG;
    if (Mix_Init(format) & format != format)
    {
        print("sdl_mixer failed");
        return 0;
    }
    else
        print("sdl_mixer ok");

    //----------------------------------------

    print("initializing audio device...");
    if (Mix_OpenAudio(44100, AUDIO_S32SYS, 2, 4096) < 0)
    {
        print("audio device failed");
        return 0;
    }
    else
        print("audio device ok");

    //----------------------------------------

    print("createing window...");
    window = SDL_CreateWindow("Goal Dash", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen::resolution.x,
                              Screen::resolution.y, 0);
    if (!window)
    {
        print("failed to create window");
        return 0;
    }
    else
        print("window ok");

    //----------------------------------------

    print("createing renderer...");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        print("failed to create renderer");
        return 0;
    }
    else
        print("renderer ok");

    //----------------------------------------

    print("turned on blend mode");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //----------------------------------------

    return 1;
}

bool Game::LoadMedia()
{
    //----------------------------------------

    if (!LoadSprite("coin", "img/coin.png", 5, Vector2(16)))
        return 0;
    if (!LoadSprite("idle", "img/idle.png", 10, Vector2(48)))
        return 0;
    if (!LoadSprite("run", "img/run.png", 9, Vector2(48)))
        return 0;
    if (!LoadSprite("jump", "img/jump.png", 4, Vector2(48)))
        return 0;
    if (!LoadSprite("spawn", "img/spawn.png", 6, Vector2(32)))
        return 0;
    if (!LoadSprite("win", "img/win.png", 6, Vector2(32)))
        return 0;
    if (!LoadSprite("wall", "img/wall.png", 1, Vector2(160)))
        return 0;
    if (!LoadSprite("healthbar", "img/healthbar.png", 5, Vector2(48, 7)))
        return 0;
    if (!LoadSprite("circ", "img/circ.png", 5, Vector2(600)))
        return 0;
    if (!LoadSprite("toggle", "img/toggle.png", 5, Vector2(787, 520)))
        return 0;

    //----------------------------------------

    if (!Background::loadBackground("bg_cloud", "img/bg_cloud.png", 1, Vector2(4096), 1.25))
        return 0;
    if (!Background::loadBackground("bg_star1", "img/bg_star.png", 1, Vector2(4096), 1.5))
        return 0;
    if (!Background::loadBackground("bg_star", "img/bg_star.png", 1, Vector2(4096), 2))
        return 0;

    //----------------------------------------

    if (!LoadSound("coin", "sound/coin.ogg"))
        return 0;
    if (!LoadSound("jump", "sound/jump.ogg"))
        return 0;
    if (!LoadSound("run", "sound/run.ogg"))
        return 0;
    if (!LoadSound("fall", "sound/fall.ogg"))
        return 0;
    if (!LoadSound("click", "sound/click.ogg"))
        return 0;
    if (!LoadSound("hover", "sound/hover.ogg"))
        return 0;
    if (!LoadSound("win", "sound/win.ogg"))
        return 0;
    if (!LoadSound("spawn", "sound/spawn.ogg"))
        return 0;
    if (!LoadMusic("bg_music", "sound/bg_music.ogg"))
        return 0;

    //----------------------------------------

    return 1;
}

void Game::Quit()
{
    //----------------------------------------

    if (player)
    {
        print("deleting player...");
        delete player;
        player = nullptr;
        print("player deleted");
    }

    //----------------------------------------

    if (!Map::Tiles.empty())
    {
        print("deleting tiles....");
        for (int i = 0; i < Screen::map_size; i++)
        {
            for (int j = 0; j < Screen::map_size; j++)
            {
                if (!Map::Tiles[i][j].second)
                    continue;
                delete Map::Tiles[i][j].second;
                Map::Tiles[i][j].second = nullptr;
            }
        }
        print("tiles deleted");
    }

    //----------------------------------------

    print("deleting sprites...");
    for (auto &sprite : Sprites)
    {
        if (!sprite.second)
            continue;
        std::string path = sprite.second->path;
        delete sprite.second;
        sprite.second = nullptr;
        print("deleted", path);
    }
    print("sprites deleted");

    //----------------------------------------

    print("deleting sounds and musics...");
    for (auto &sound : Sounds)
    {
        if (!sound.second)
            continue;
        Mix_FreeChunk(sound.second);
        sound.second = nullptr;
        print("deleted", sound.first);
    }

    for (auto &music : Musics)
    {
        if (!music.second)
            continue;
        Mix_FreeMusic(music.second);
        music.second = nullptr;
        print("deleted", music.first);
    }

    print("sounds and music deleted");

    //----------------------------------------

    if (myFont)
    {
        print("deleting font...");
        TTF_CloseFont(myFont);
        print("font deleted");
    }

    //----------------------------------------

    UI::RemovingUIs();
    UI::RemoveUIs();

    //----------------------------------------

    print("closing sdl2...");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    print("sdl2 closed");

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
    for (int i = sqr; i < resolution.y; i += sqr)
    {
        for (int j = sqr; j < resolution.x; j += sqr)
        {
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void Screen::SetWindowSize()
{
    SDL_SetWindowSize(window, resolutions[current_resolution], resolutions[current_resolution]);

    Screen::resolution = Vector2(Screen::resolutions[Screen::current_resolution]);

    Screen::resolution = Int((Screen::resolution / Screen::map_size)) * Screen::map_size;

    Screen::tile_size = Screen::resolution.x / Screen::map_size;

    Game::properties["gravity"].f = Clamp(Game::properties["gravity"].f, 0.0f, 10.0f);

    Game::properties["gravity"].f *= Screen::resolution.x / 25100.0f;

    Game::properties["player_jump_speed"].f = Clamp(Game::properties["player_jump_speed"].f, 0.5f, 2.5f);

    Game::properties["player_jump_speed"].f *= Screen::resolution.x / 82.0f;

    Game::properties["player_move_speed"].f = Clamp(Game::properties["player_move_speed"].f, 0.5f, 2.5f);

    Game::properties["player_move_speed"].f *= Screen::resolution.x / 190.0f;

    Game::properties["player_acceleration"].f = Clamp(Game::properties["player_acceleration"].f, 0.25f, 2.0f);

    Game::properties["player_acceleration"].f *= Screen::resolution.x / 20000.0f;
}