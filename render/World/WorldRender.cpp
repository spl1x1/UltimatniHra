//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

#include "../../server/World/generace_mapy.h"

void WorldRender::GenerateWorld(int seed, Window& window) {
    WorldData worldData;

    GeneraceMapy *generaceMapy = new GeneraceMapy();

    //TODO: implementovat přímo do generace generace mapy
    for (int x = 0; x < generaceMapy->biomMapa.size(); x++) {
        for (int y = 0; y < generaceMapy->biomMapa.at(x).size(); y++) {
            worldData.biomeMap[x][y] = generaceMapy->biomMapa.at(x).at(y);
        }
    }
    delete generaceMapy;
    WorldData::getBlockVariationMap(worldData);

    GenerateTexture(window, worldData);
    window.worldData = worldData;
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

    SDL_DestroySurface(window.surfaces["Snow1.bmp"]);
    window.surfaces.erase("Snow1.bmp");

    SDL_DestroySurface(window.surfaces["Snow2.bmp"]);
    window.surfaces.erase("Snow2.bmp");

    SDL_DestroySurface(window.surfaces["Snow3.bmp"]);
    window.surfaces.erase("Snow3.bmp");

    SDL_DestroySurface(window.surfaces["beach.bmp"]);
    window.surfaces.erase("beach.bmp");

    SDL_DestroySurface(window.surfaces["sand.bmp"]);
    window.surfaces.erase("sand.bmp");

    SDL_DestroySurface(window.surfaces["mountain.bmp"]);
    window.surfaces.erase("mountain.bmp");

    SDL_DestroySurface(window.surfaces["forest.bmp"]);
    window.surfaces.erase("forest.bmp");

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

void WorldRender::GenerateTexture(Window& window, WorldData& worldData) {
    for (const auto& entry : std::filesystem::directory_iterator("assets/textures/world")) {
        std::string fileName = entry.path().string();
        SDL_Log("Directory:: %s", fileName.c_str());
        loadSurfacesFromDirectory(fileName, window);
    }
    SDL_Surface* finalSurface = SDL_CreateSurface(512*TEXTURERES,512*TEXTURERES,SDL_PIXELFORMAT_ABGR8888);

    for (int x = 0; x < MAPSIZE; x++) {
        for (int y = 0; y < MAPSIZE; y++) {
            int tileType = worldData.biomeMap[x][y];
            SDL_Rect destRect;
            destRect.x = x * TEXTURERES;
            destRect.y = y * TEXTURERES;
            destRect.w = TEXTURERES;
            destRect.h = TEXTURERES;

            SDL_Surface* srcSurface = nullptr;
            switch (tileType) {
                case 1:
                {
                    srcSurface = window.surfaces["beach.bmp"];
                    break;
                }
                case 2:
                {
                    srcSurface = window.surfaces["sand.bmp"];
                    break;
                }
                case 3:
                {
                    std::string variationTexture = "grass" + std::to_string(worldData.blockVariantionMap[x][y]) + ".bmp";
                    srcSurface = window.surfaces[variationTexture];
                    break;
                }
                case 4:
                {
                    srcSurface = window.surfaces["mountain.bmp"];
                    break;
                }
                case 5:
                {
                    srcSurface = window.surfaces["forest.bmp"];
                    break;
                }
                case 6:
                {
                    std::string variationTexture = "Snow" + std::to_string(worldData.blockVariantionMap[x][y]) + ".bmp";
                    srcSurface = window.surfaces[variationTexture];
                    break;
                }
                default: break;
            }
            SDL_BlitSurface(srcSurface, nullptr, finalSurface, &destRect);

        }
    }
    window.surfaces["WorldMap"] = finalSurface;
    window.CreateTextureFromSurface("WorldMap","WorldMap");
    SDL_SaveBMP(finalSurface, "assets/worldmap.bmp");
    //ReleaseResources(window);
};
