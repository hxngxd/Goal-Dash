#include "datalib/msg.h"
#include "event/ui.h"
#include "game.h"
#include <fstream>
#include <regex>

bool Game::LoadConfig()
{
    //----------------------------------------

    std::ifstream file(".config");
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
                }
                else if (type == "float")
                {
                    prop.f = std::stof(value);
                }
                else if (type == "bool")
                {
                    if (value == "false" || value == "0")
                        prop.b = false;
                    else if (value == "true" || value == "1")
                        prop.b = true;
                    else
                        throw("type");
                }
                else if (type == "string")
                {
                    prop.s = value;
                }
                else
                {
                    throw("type");
                }
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
            Properties[key] = prop;
        }
    }

    //----------------------------------------

    Screen::resolution = Vector2(Game::Properties["resolution"].i);
    Screen::tile_size = Screen::resolution.x / Screen::map_size;
    Game::Properties["gravity"].f *= Screen::resolution.x / 25100.0f;
    Game::Properties["player_jump_speed"].f *= Screen::resolution.x / 80.0f;
    Game::Properties["player_move_speed"].f *= Screen::resolution.x / 180.0f;
    Game::Properties["player_acceleration"].f *= Screen::resolution.x / 20000.0f;

    //----------------------------------------

    file.close();
    return 1;
}