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
int Screen::font_size;
std::vector<int> Screen::resolutions = {480, 640, 800, 960, 1120};
int Screen::current_resolution;

void Game::Update()
{
    //----------------------------------------

    Screen::Clear(Color::black());

    if (properties["point_grid"].b)
        Screen::PointGrid(Color::white());

    //----------------------------------------

    EventHandler::Update();

    if (player)
    {
        if (Game::properties["show_time"].b)
            Text::SetLabel("Play-0.time", "Time: " + FormatMS(SDL_GetTicks() - time[0]));

        if (properties["background_enable"].b)
        {
            Background::MoveRelativeTo(player->position);
            Background::Update();
        }
    }
    else
    {
        if (properties["background_enable"].b)
        {
            Background::MoveRelativeTo(EventHandler::MousePosition);
            Background::Update();
        }
    }

    Map::Update();

    if (player)
        player->Update();

    if (Map::mode == 1)
        MapMaking::Update();

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
    if (!LoadConfig(0))
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

    if (properties["music"].s != "off")
    {
        print("playing background music...");
        PlayMusic(-1);
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

    Screen::CalculateGravity();
    Screen::CalculateMoveSpeed();
    Screen::CalculateJumpSpeed();
    Screen::CalculateAcceleration();

    //----------------------------------------

    print("changing cursor");
    if (!ChangeCursor("img\\cursor.png"))
    {
        print("failed to change cursor");
        return;
    }
    print("cursor changed");

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
    int format = MIX_INIT_OGG | MIX_INIT_MP3;
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

    if (!Background::loadBackground("img/bg_cloud.png", Vector2(4096)))
        return 0;
    if (!Background::loadBackground("img/bg_star.png", Vector2(4096)))
        return 0;
    if (!Background::loadBackground("img/bg_star1.png", Vector2(4096)))
        return 0;

    //----------------------------------------

    std::vector<std::string> names = {"coin", "jump", "run", "fall", "click", "hover", "win", "spawn", "die"};
    for (auto &name : names)
    {
        if (!LoadSound(name, "sound/" + name + ".ogg"))
            return 0;
    }

    if (properties["music"].s != "off")
    {
        if (!LoadMusic(properties["music"].s))
            return 0;
    }

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

    print("deleting backgrounds...");
    for (auto &bg : Background::Backgrounds)
    {
        delete bg;
        bg = nullptr;
    }
    Background::Backgrounds.clear();
    print("backgrounds deleted");

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

    if (Music)
    {
        Mix_FreeMusic(Music);
        Music = nullptr;
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

    if (myCursor)
    {
        print("deleting cursor...");
        SDL_FreeCursor(myCursor);
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