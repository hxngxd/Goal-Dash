#include "datalib/msg.h"
#include "event/ui.h"
#include "game.h"
#include <fstream>
#include <regex>

float Screen::tmpGravity, Screen::tmpMoveSpeed, Screen::tmpJumpSpeed, Screen::tmpAcceleration;

bool Game::LoadConfig(bool is_default)
{
    //----------------------------------------

    std::ifstream file;
    if (is_default)
        file.open("config/.default");
    else
        file.open("config/.current");

    if (!file.good())
    {
        print("config file not found");
        return 0;
    }
    if (!file.is_open())
    {
        print("can't open config file");
        file.close();
        return 0;
    }

    //----------------------------------------

    std::string line;
    std::regex pattern(R"(\<(\w+)\>\s+\{(\w+)=(.*)\})");
    std::getline(file, line);
    if (line != "#config")
    {
        print("config file is not valid");
        return 0;
    }

    //----------------------------------------

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::smatch matches;
        if (!std::regex_match(line, matches, pattern))
        {
            print("invalid property");
            return 0;
        }
        else
        {
            std::string type = matches[1];
            std::string key = matches[2];
            std::string value = matches[3];
            if (type.empty() || key.empty() || value.empty())
            {
                print("invalid property");
                return 0;
            }
            PropertiesType prop;
            try
            {
                if (type == "int")
                {
                    prop.i = std::stoi(value);
                    prop.type = INT;
                }
                else if (type == "float")
                {
                    prop.f = std::stof(value);
                    prop.type = FLOAT;
                }
                else if (type == "bool")
                {
                    if (value == "false" || value == "0")
                    {
                        prop.b = false;
                        prop.type = BOOL;
                    }
                    else if (value == "true" || value == "1")
                    {
                        prop.b = true;
                        prop.type = BOOL;
                    }
                    else
                        throw("type");
                }
                else if (type == "string")
                {
                    prop.s = value;
                    prop.type = STR;
                }
                else
                    throw("type");
            }
            catch (std::invalid_argument const &e)
            {
                print("invalid value");
                return 0;
            }
            catch (std::out_of_range const &e)
            {
                print("invalid value");
                return 0;
            }
            catch (std::string &e)
            {
                print("invalid", e);
                return 0;
            }
            properties[key] = prop;
        }
    }

    //----------------------------------------

    Screen::tmpMoveSpeed = properties["player_move_speed"].f;
    Screen::tmpJumpSpeed = properties["player_jump_speed"].f;
    Screen::tmpAcceleration = properties["player_acceleration"].f;
    Screen::tmpGravity = properties["gravity"].f;

    Screen::map_size = 16;

    Screen::SetWindowSize();

    properties["ray_opacity"].i = Clamp(properties["ray_opacity"].i, 0, 255);

    properties["tile_rescale_speed"].f = Clamp(properties["tile_rescale_speed"].f, 0.01f, 0.1f);

    properties["tile_scale"].f = Clamp(properties["tile_scale"].f, 0.1f, 1.0f);

    properties["volume"].i = Clamp(properties["volume"].i, 0, 128);

    //----------------------------------------

    file.close();
    return 1;
}

void Game::SaveConfig()
{
    std::ofstream out;
    out.open("config/.current");
    if (!out.good())
    {
        print("failed to save config");
        out.close();
        return;
    }

    out << "#config\n\n";

    properties["player_move_speed"].f = Screen::tmpMoveSpeed;
    properties["player_jump_speed"].f = Screen::tmpJumpSpeed;
    properties["player_acceleration"].f = Screen::tmpAcceleration;
    properties["gravity"].f = Screen::tmpGravity;

    for (auto &prop : properties)
    {
        out << "<";
        switch (prop.second.type)
        {
        case INT:
            out << "int> {" << prop.first << "=" << prop.second.i;
            break;
        case STR:
            out << "string> {" << prop.first << "=" << prop.second.s;
            break;
        case FLOAT:
            out << "float> {" << prop.first << "=" << prop.second.f;
            break;
        case BOOL:
            out << "bool> {" << prop.first << "=" << prop.second.b;
            break;
        }
        out << "}\n";
    }

    out.close();

    Screen::CalculateGravity();
    Screen::CalculateMoveSpeed();
    Screen::CalculateJumpSpeed();
    Screen::CalculateAcceleration();

    print("config saved");

    return;
}

void Screen::SetWindowSize()
{
    Screen::current_resolution = properties["resolution"].i =
        Clamp(properties["resolution"].i, 0, (int)(resolutions.size() - 1));

    SDL_SetWindowSize(window, resolutions[current_resolution], resolutions[current_resolution]);

    Screen::resolution = Vector2(Screen::resolutions[Screen::current_resolution]);

    Screen::resolution = Int((Screen::resolution / Screen::map_size)) * Screen::map_size;

    Screen::tile_size = Screen::resolution.x / Screen::map_size;

    Screen::font_size = Screen::resolution.x / 48.0f;
}

void Screen::CalculateGravity()
{
    tmpGravity = Game::properties["gravity"].f = Clamp(Game::properties["gravity"].f, 0.0f, 10.0f);

    Game::properties["gravity"].f *= Screen::resolution.x / 25100.0f;
}

void Screen::CalculateMoveSpeed()
{
    tmpMoveSpeed = Game::properties["player_move_speed"].f = Clamp(Game::properties["player_move_speed"].f, 0.5f, 2.5f);

    Game::properties["player_move_speed"].f *= Screen::resolution.x / 190.0f;
}

void Screen::CalculateJumpSpeed()
{
    tmpJumpSpeed = Game::properties["player_jump_speed"].f = Clamp(Game::properties["player_jump_speed"].f, 0.5f, 2.5f);

    Game::properties["player_jump_speed"].f *= Screen::resolution.x / 82.0f;
}

void Screen::CalculateAcceleration()
{
    tmpAcceleration = Game::properties["player_acceleration"].f =
        Clamp(Game::properties["player_acceleration"].f, 0.25f, 2.0f);

    Game::properties["player_acceleration"].f *= Screen::resolution.x / 20000.0f;
}