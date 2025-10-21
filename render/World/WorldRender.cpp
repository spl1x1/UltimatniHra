//
// Created by USER on 21.10.2025.
//

#include "WorldRender.h"

void WorldRender::loadTexturesFromDirectory(const std::string& directoryPath, Window& window) {
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        std::cout << "Loading texture: " << entry << std::endl;
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

    for (int x = 0; x < window.worldData.WorldMap.size(); x++) {
        for (int y = 0; y < window.worldData.WorldMap.at(x).size(); y++) {
            int tileType = window.worldData.WorldMap.at(x).at(y);
            SDL_Rect destRect;
            destRect.x = x * 16;
            destRect.y = y * 16;
            destRect.w = 16;
            destRect.h = 16;

            SDL_Surface* srcSurface = SDL_CreateSurface(16,16,SDL_PIXELFORMAT_ABGR8888);
            switch (tileType) {
                case 1:
                {
                    srcSurface = window.surfaces[R"(assets/textures/world\grass\grass_1.bmp)"];
                    break;
                }
                case 2:
                {
                    srcSurface = window.surfaces[R"(assets/textures/world\grass\grass_2.bmp)"];
                    break;
                }
                case 3:
                {
                    srcSurface = window.surfaces[R"(assets/textures/world/grass/grass_3.bmp)"];
                    break;
                }
                case 4:
                {
                    srcSurface = window.surfaces[R"(assets/textures/world/grass/grass_4.bmp)"];
                    break;
                }
                case 5:
                {
                    srcSurface = window.surfaces[R"(assets/textures/world\grass\grass_5.bmp)"];
                    break;
                }
                case 6:
                {
                    srcSurface = window.surfaces[R"(assets/textures/world\grass\grass_1.bmp)"];
                    break;
                }
                default:
                {
                    srcSurface =  window.surfaces["assets/Sprite-0001.bmp"];
                    break;
                }
            }
            SDL_BlitSurface(srcSurface, nullptr, finalSurface, &destRect);

        }
    }
    window.surfaces["WorldMap"] = finalSurface;
    window.CreateTextureFromSurface("WorldMap","WorldMap");
    SDL_SaveBMP(finalSurface, "assets/worldmap.bmp");
};
