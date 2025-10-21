//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

void WorldRender::GenerateTexture(Window& window, std::pmr::vector<std::pmr::vector<int>>& worldMap) {
    window.LoadTexture("assets/textures/Grass.bmp");



    for (int x = 0; x < worldMap.size(); x++) {
        for (int y = 0; y < worldMap[x].size(); y++) {
            int tileType = worldMap[x][y];
            std::string texturePath;
        }
    }
};
