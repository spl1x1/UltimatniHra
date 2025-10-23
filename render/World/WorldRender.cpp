//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

void WorldRender::loadTextures(Window &window) {
    window.CreateTextureFromSurface("Water1.bmp", "Water1");
    window.CreateTextureFromSurface("Water2.bmp", "Water2");
    window.CreateTextureFromSurface("Water3.bmp", "Water3");
    window.CreateTextureFromSurface("Water4.bmp", "Water4");
    window.CreateTextureFromSurface("Water5.bmp", "Water5");

}

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

    SDL_DestroySurface(window.surfaces["WorldMap"]);
    window.surfaces.erase("WorldMap");
}


void WorldRender::loadSurfacesFromDirectory(const std::string& directoryPath, Window& window) {
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
        loadSurfacesFromDirectory(fileName, window);
    }
    SDL_Surface* finalSurface = SDL_CreateSurface(512*TEXTURERES,512*TEXTURERES,SDL_PIXELFORMAT_ABGR8888);

    for (int x = 0; x < window.worldData.WorldMap.size(); x++) {
        for (int y = 0; y < window.worldData.WorldMap.at(x).size(); y++) {
            int tileType = window.worldData.WorldMap.at(x).at(y);
            SDL_Rect destRect;
            destRect.x = x * TEXTURERES;
            destRect.y = y * TEXTURERES;
            destRect.w = TEXTURERES;
            destRect.h = TEXTURERES;

            SDL_Surface* srcSurface = nullptr;
            switch (tileType) {
                case 1:
                {
                    srcSurface = window.surfaces["sand.bmp"];
                    break;
                }
                case 2:
                {
                    srcSurface = window.surfaces["sand.bmp"];
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
                default: break;
            }
            SDL_BlitSurface(srcSurface, nullptr, finalSurface, &destRect);

        }
    }
    window.surfaces["WorldMap"] = finalSurface;
    window.CreateTextureFromSurface("WorldMap","WorldMap");
    loadTextures(window);
    SDL_SaveBMP(finalSurface, "assets/worldmap.bmp");
    ReleaseResources(window);
};
