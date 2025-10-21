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
    SDL_Surface* finalSurface = SDL_CreateSurface(8192,8192,SDL_PIXELFORMAT_ABGR8888);

    for (int x = 0; window.worldData.WorldMap.size(); x++) {
        for (int y = 0; y < window.worldData.WorldMap[x].size(); y++) {
            int tileType = window.worldData.WorldMap[x][y];
            SDL_Rect destRect;
            destRect.x = x * 16;
            destRect.y = y * 16;
            destRect.w = 16;
            destRect.h = 16;

            SDL_Surface* srcSurface = window.surfaces["assets/Sprite-0001.bmp"];
            switch (tileType) {
                case 1:
                {
                    srcSurface = nullptr; // TODO: přiřadit skutečný povrch pro PLAZ
                    break;
                }
                case 2:
                {
                    srcSurface = nullptr; // TODO: přiřadit skutečný povrch pro POUST
                    break;
                }
                case 3:
                {
                    srcSurface = nullptr; // TODO: přiřadit skutečný povrch pro TRAVA
                    break;
                }
                case 4:
                {
                    srcSurface = nullptr; // TODO: přiřadit skutečný povrch pro SKALA
                    break;
                }
                case 5:
                {
                    srcSurface = nullptr; // TODO: přiřadit skutečný povrch pro LES
                    break;
                }
                case 6:
                {
                    srcSurface = nullptr; // TODO: přiřadit skutečný povrch pro SNIH
                    break;
                }
                default:
                {
                    srcSurface = nullptr;
                    break;
                }
            }

            SDL_BlitSurface(srcSurface, nullptr, finalSurface, &destRect);
        }
    }
};
