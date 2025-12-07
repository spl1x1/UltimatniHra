//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

#include <SDL3_image/SDL_image.h>


void WorldRender::GenerateTextures() {
    window.server->generateWorld();
    window.loadSurfacesFromDirectory("assets/textures/world");
    GenerateWorldTexture();
    GenerateWaterTextures();
    ReleaseResources();
}

void WorldRender::GenerateStructureTextures() const {


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

void WorldRender::GenerateWaterTextures() const {
    std::vector<WaterSurface> waterFrames;
    int spriteCount = 4;
    std::string textureName = "water.bmp";

    for (int frame = 1; frame <= spriteCount; frame++) {
        std::unique_ptr<SDL_Rect> rect = std::make_unique<SDL_Rect>();
        rect->x = (frame - 1) * TEXTURERES;
        rect->y = 0;
        rect->w = TEXTURERES;
        rect->h = TEXTURERES;

        auto *surface = SDL_CreateSurface(768,512,SDL_PIXELFORMAT_ABGR8888);
        waterFrames.push_back(WaterSurface{frame, "water_"+std::to_string(frame),surface, std::move(rect)});
    }
    auto srcSurface = window.surfaces["water.bmp"];

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 24; j++) {
            SDL_Rect destRect;
            destRect.x = j * 32;
            destRect.y = i * 32;
            destRect.w = 32;
            destRect.h = 32;

            for (int frame = 0; frame <= spriteCount-1; frame++) {
                auto targetSurface = waterFrames.at(frame).surface;

                SDL_BlitSurface(srcSurface,waterFrames.at(frame).rect.get(), targetSurface, &destRect);
            }
        }
    }

    for (const auto& waterFrame : waterFrames) {
        window.surfaces[waterFrame.textureName] = waterFrame.surface;
        window.CreateTextureFromSurface(waterFrame.textureName,waterFrame.textureName);
        IMG_SavePNG(waterFrame.surface, ("assets/" + waterFrame.textureName + ".png").c_str());
    }
}

void WorldRender::GenerateWorldTexture() const {
    SDL_Surface* finalSurface = SDL_CreateSurface(512*TEXTURERES,512*TEXTURERES,SDL_PIXELFORMAT_ABGR8888);

    for (int x = 0; x < MAPSIZE; x++) {
        for (int y = 0; y < MAPSIZE; y++) {
            int tileType = window.server->getCollisionMapValue(x,y, WorldData::BIOME_MAP);
            int variation = window.server->getCollisionMapValue(x,y, WorldData::BLOCK_VARIATION_MAP);

            SDL_Rect destRect;
            destRect.x = x * TEXTURERES;
            destRect.y = y * TEXTURERES;
            destRect.w = TEXTURERES;
            destRect.h = TEXTURERES;

            SDL_Rect srcRect;
            srcRect.x = variation * TEXTURERES;
            srcRect.y = 0;
            srcRect.w = TEXTURERES;
            srcRect.h = TEXTURERES;

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
                    srcSurface = window.surfaces["grass.bmp"];
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
                    srcSurface = window.surfaces["snow.bmp"];
                    break;
                }
                default: break;
            }
            SDL_BlitSurface(srcSurface, &srcRect, finalSurface, &destRect);

        }
    }
    window.surfaces["WorldMap"] = finalSurface;
    window.CreateTextureFromSurface("WorldMap","WorldMap");
    IMG_SavePNG(finalSurface, "assets/worldmap.png");
};
