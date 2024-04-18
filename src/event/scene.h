#pragma once
#include <string>

class Scene
{
  public:
    static void Welcome();
    static void Play();
    static void MapMaking();
    static void Common();
    static void SpawnPlayer();
    static void Settings();
    static void SelectMusic();
    static void SelectMap();
    static void SaveChoice();
    static void SaveAs();
    static void ShowMessage(std::string message);
};