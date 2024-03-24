#include "game.h"
#include "datalib/mixer.h"
#include "datalib/msg.h"
#include "datalib/sprite.h"
#include "datalib/util.h"
#include "event/input.h"
#include "event/ui.h"
#include "func/func.h"

Vector2 Screen::resolution(640, 640);
int Screen::map_size = 16;
int Screen::tile_size = Screen::resolution.x / Screen::map_size;

SDL_Event Game::event;
SDL_Window *Game::window = nullptr;
SDL_Renderer *Game::renderer = nullptr;
Scene *Game::scene = nullptr;
Player *Game::player = nullptr;
int Game::player_score = 0;
bool Game::player_won = false;

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

    LinkedFunction::Update();
    MapTile::Draw();
    UI::Update();
    if (player)
    {
        player->Update();
    }

    //----------------------------------------

    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_QUIT)
            running = false;
        int x, y;
        SDL_GetMouseState(&x, &y);
        mousePosition = Vector2(x, y);
        EventHandler::MouseInputHandler();
        if (player)
            EventHandler::PlayerInputHandler(player, Game::Properties["keyboard_layout"].b ? right_keys : left_keys);
    }

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

    if (Game::Properties["music"].b)
    {
        print("playing background music...");
        PlayMusic("bg_music", -1);
        Mix_VolumeMusic(Game::Properties["music_volume"].i);
    }

    //----------------------------------------

    print("loading font...");
    myFont = TTF_OpenFont(Game::Properties["font"].s.c_str(), 24);
    if (!myFont)
    {
        print("failed to load font");
        return;
    }
    print("font loaded");

    //----------------------------------------

    scene = new Scene();

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
    if (!LoadSprite("button_hover", "img/button_hover.png", 1, Vector2(695, 377)))
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

    print("deleting player...");
    if (player)
    {
        delete player;
        player = nullptr;
    }
    print("player deleted");

    //----------------------------------------

    print("deleting tiles....");
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
    print("tiles deleted");

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

    print("deleting font...");
    if (myFont)
        TTF_CloseFont(myFont);
    print("font deleted");

    //----------------------------------------

    if (scene)
    {
        scene->DeleteScene();
        scene = nullptr;
    }

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
    for (int i = sqr; i < resolution.x; i += sqr)
    {
        for (int j = sqr; j < resolution.y; j += sqr)
        {
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}