//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

void WorldRender::loadTexturesFromDirectory(const std::string& directoryPath, Window& window) {
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        std::cout << "Loading texture: " << fileName << std::endl;
        window.LoadSurface(fileName);
    }
}

void WorldRender::GenerateTexture(Window& window) {
    for (const auto& entry : std::filesystem::directory_iterator("assets/textures/world")) {
        std::string fileName = entry.path().string();
        std::cout << "Directory: " << fileName << std::endl;
        loadTexturesFromDirectory(fileName, window);
    }
   // vector<vector<int>> mapa= generaceMapy.biomMapa;


/*    for (int x = 0; x < worldMap.size(); x++) {
        for (int y = 0; y < worldMap[x].size(); y++) {
            int tileType = worldMap[x][y];
            std::string texturePath;
        }
    } */
};
