#pragma once
#include <iostream>

template <typename T> void print_(T msg)
{
    std::cout << msg << " ";
}

template <typename T, typename... Args> void print_(T msg, Args... args)
{
    std::cout << msg << " ";
    print_(args...);
}

template <typename... Args> void print(Args... args)
{
    print_(args...);
    std::cout << std::endl;
}