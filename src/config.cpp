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
                    prop.i = std::stoi(value);
                else if (type == "float")
                    prop.f = std::stof(value);
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
                    prop.s = value;
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

    Screen::map_size = 16;

    Screen::current_resolution = Game::properties["resolution"].i;

    Screen::SetWindowSize();

    Game::properties["ray_opacity"].i = Clamp(Game::properties["ray_opacity"].i, 0, 255);

    Game::properties["tile_rescale_speed"].f = Clamp(Game::properties["tile_rescale_speed"].f, 0.01f, 0.1f);

    Game::properties["tile_scale"].f = Clamp(Game::properties["tile_scale"].f, 0.1f, 1.0f);

    Game::properties["volume"].i = Clamp(Game::properties["volume"].i, 0, 128);

    //----------------------------------------

    file.close();
    return 1;
}