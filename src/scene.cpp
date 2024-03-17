#include "game.h"
#include "gameobject.h"

Scene::Scene()
{
    MapTile::CreateBorder();
}

Scene::Scene(std::string map)
{
    MapTile::CreateTiles(map);
}

Scene::~Scene()
{
    MapTile::DeleteTiles();
}

void Scene::Update()
{
    MapTile::Draw();
}