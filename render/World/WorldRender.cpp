//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

void WorldRender::ReleaseResources(Window &window) {
    SDL_DestroySurface(window.surfaces["grass_1.bmp"]);
    window.surfaces.erase("grass_1.bmp");

    SDL_DestroySurface(window.surfaces["grass_2.bmp"]);
    window.surfaces.erase("grass_2.bmp");

    SDL_DestroySurface(window.surfaces["grass_3.bmp"]);
    window.surfaces.erase("grass_3.bmp");

    SDL_DestroySurface(window.surfaces["grass_4.bmp"]);
    window.surfaces.erase("grass_4.bmp");

    SDL_DestroySurface(window.surfaces["grass_5.bmp"]);
    window.surfaces.erase("grass_5.bmp");
}


void WorldRender::loadTexturesFromDirectory(const std::string& directoryPath, Window& window) {
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        SDL_Log("Loading surface: %s", fileName.c_str());
        window.LoadSurface(fileName,entry.path().filename().string());
    }
}

void WorldRender::GenerateTexture(Window& window) {
    for (const auto& entry : std::filesystem::directory_iterator("assets/textures/world")) {
        std::string fileName = entry.path().string();
        SDL_Log("Directory:: %s", fileName.c_str());
        loadTexturesFromDirectory(fileName, window);
    }
    SDL_Surface* finalSurface = SDL_CreateSurface(8192,8192,SDL_PIXELFORMAT_ABGR8888);

    for (int x = 0; x < window.worldData.WorldMap.size(); x++) {
        for (int y = 0; y < window.worldData.WorldMap.at(x).size(); y++) {
            int tileType = window.worldData.WorldMap.at(x).at(y);
            SDL_Rect destRect;
            destRect.x = x * 16;
            destRect.y = y * 16;
            destRect.w = 16;
            destRect.h = 16;

            SDL_Surface* srcSurface = nullptr;
            switch (tileType) {
                case 1:
                {
                    srcSurface = window.surfaces["grass_1.bmp"];
                    break;
                }
                case 2:
                {
                    srcSurface = window.surfaces["grass_2.bmp"];
                    break;
                }
                case 3:
                {
                    srcSurface = window.surfaces["grass_3.bmp"];
                    break;
                }
                case 4:
                {
                    srcSurface = window.surfaces["grass_4.bmp"];
                    break;
                }
                case 5:
                {
                    srcSurface = window.surfaces["grass_5.bmp"];
                    break;
                }
                case 6:
                {
                    srcSurface = window.surfaces["grass_1.bmp"];
                    break;
                }
                default:
                {
                    srcSurface =  nullptr;
                    break;
                }
            }
            SDL_BlitSurface(srcSurface, nullptr, finalSurface, &destRect);

        }
    }
    window.surfaces["WorldMap"] = finalSurface;
    window.CreateTextureFromSurface("WorldMap","WorldMap");
    SDL_SaveBMP(finalSurface, "assets/worldmap.bmp");
    ReleaseResources(window);
};
