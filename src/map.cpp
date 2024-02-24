#include "game.h"

Map::Map(int rows, int cols, std::vector<float> tileProbabilities, std::vector<std::vector<int>> & tileMap){
    this->rows = rows;
    this->cols = cols;

    tileMap.resize(rows, std::vector<int>(cols, 0));

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::discrete_distribution<> distribution(tileProbabilities.begin(), tileProbabilities.end());

    for (int i=0;i<rows;i++){
        int r = empty;
        while (r==empty) r = distribution(gen);
        tileMap[i][0] = tileMap[i][cols-1] = r; 
    }
    for (int i=1;i<cols-1;i++){
        int r = empty;
        while (r==empty) r = distribution(gen);
        tileMap[0][i] = tileMap[rows-1][i] = r;
    }
    for (int i=1;i<rows-1;i++){
        for (int j=1;j<cols-1;j++){
            tileMap[i][j] = distribution(gen);
        }
    }
}

void Map::Draw(std::vector<std::vector<int>> & tileMap){
    int tile_size = resolution.x/map_size;
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            if (!tileMap[i][j]) continue;
            SDL_Rect rect = {j*tile_size, i*tile_size, tile_size, tile_size};
            switch (tileMap[i][j]){
                case 1:
                    Renderer::SetDrawColor(Color::white(255));
                    break;
                case 2:
                    Renderer::SetDrawColor(Color::red(255));
                    break;
                default:
                    break;
            }
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}