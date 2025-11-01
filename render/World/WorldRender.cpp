//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

#include <SDL3_image/SDL_image.h>


void WorldRender::GenerateTextures() {
    window.server.generateWorld();
    GenerateWorldTexture();
    GenerateWaterTextures();
    ReleaseResources();

}


void WorldRender::ReleaseResources() const {
    for (auto it = window.surfaces.begin(); it != window.surfaces.end(); ) {
        if (it->first.empty()) {
            ++it;
            continue;
        }
        SDL_Log("Releasing surface: %s", it->first.c_str());
        SDL_DestroySurface(it->second);
        it = window.surfaces.erase(it);
    }
}


void WorldRender::loadSurfacesFromDirectory(const std::string& directoryPath) const {
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        SDL_Log("Loading surface: %s", fileName.c_str());
        window.LoadSurface(fileName,entry.path().filename().string());
    }
}

void WorldRender::GenerateWaterTextures() const {
    std::vector<WaterSurface> waterFrames;
    int spriteCount = window.sprites_.at(0).getFrameCount();

    for (int frame = 1; frame <= spriteCount; frame++) {
        WaterSurface waterSurface;
        waterSurface.id = frame;
        waterSurface.textureName = "water" + std::to_string(frame) + ".bmp";;
        waterSurface.surface = SDL_CreateSurface(768,512,SDL_PIXELFORMAT_ABGR8888);
        waterFrames.push_back(waterSurface);
    }

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 24; j++) {
            SDL_Rect destRect;
            destRect.x = j * 32;
            destRect.y = i * 32;
            destRect.w = 32;
            destRect.h = 32;

            for (int frame = 0; frame <= spriteCount-1; frame++) {
                auto srcSurface = window.surfaces[waterFrames.at(frame).textureName];
                auto targetSurface = waterFrames.at(frame).surface;

                SDL_BlitSurface(srcSurface, nullptr, targetSurface, &destRect);
            }
        }
    }

    for (const auto& waterFrame : waterFrames) {
        window.surfaces[waterFrame.textureName] = waterFrame.surface;
        window.CreateTextureFromSurface(waterFrame.textureName, "water" + std::to_string(waterFrame.id));
        IMG_SavePNG(waterFrame.surface, ("assets/water" + std::to_string(waterFrame.id) + ".png").c_str());
    }
}

void WorldRender::GenerateWorldTexture() const {
    for (const auto& entry : std::filesystem::directory_iterator("assets/textures/world")) {
        std::string fileName = entry.path().string();
        SDL_Log("Directory:: %s", fileName.c_str());
        loadSurfacesFromDirectory(fileName);
    }
    SDL_Surface* finalSurface = SDL_CreateSurface(512*TEXTURERES,512*TEXTURERES,SDL_PIXELFORMAT_ABGR8888);

    for (int x = 0; x < MAPSIZE; x++) {
        for (int y = 0; y < MAPSIZE; y++) {
            int tileType = window.server.worldData.biomeMap[x][y];
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
                    std::string variationTexture = "grass" + std::to_string(window.server.worldData.blockVariantionMap[x][y]) + ".bmp";
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
                    std::string variationTexture = "Snow" + std::to_string(window.server.worldData.blockVariantionMap[x][y]) + ".bmp";
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
    IMG_SavePNG(finalSurface, "assets/worldmap.png");
};
