#include "game.h"

bool Game::LoadConfig()
{
    //----------------------------------------

    std::ifstream file(".config");
    if (!file.good())
    {
        ShowMsg(2, fail, "'.config' not found.");
        return 0;
    }
    if (!file.is_open())
    {
        ShowMsg(2, fail, "can't open .config.");
        file.close();
        return 0;
    }

    //----------------------------------------

    std::string line;
    std::regex pattern(R"(\<(\w+)\>\s+\{(\w+)=(.*)\})");
    std::getline(file, line);
    if (line != "#config")
    {
        ShowMsg(2, fail, "config file is not valid.");
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
            ShowMsg(2, fail, "invalid property.");
            return 0;
        }
        else
        {
            std::string type = matches[1];
            std::string key = matches[2];
            std::string value = matches[3];
            if (type.empty() || key.empty() || value.empty())
            {
                ShowMsg(2, fail, "invalid property.");
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
                ShowMsg(2, fail, "invalid value.");
                return 0;
            }
            catch (std::out_of_range const &e)
            {
                ShowMsg(2, fail, "invalid value.");
                return 0;
            }
            catch (std::string &e)
            {
                ShowMsg(2, fail, "invalid " + e + ".");
                return 0;
            }
            Properties[key] = prop;
        }
    }

    //----------------------------------------

    file.close();
    return 1;
}