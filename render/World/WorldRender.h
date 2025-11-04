//
// Created by USER on 21.10.2025.
//

#ifndef PREPAREWORLD_H
#define PREPAREWORLD_H

#ifndef TEXTURERES
#define TEXTURERES 32
#endif

#include "../Window.h"
#include <filesystem>
#include <SDL3/SDL.h>
#include <string>



struct WaterSurface {
    int id;
    std::string textureName;
    SDL_Surface* surface;
    const SDL_Rect *rect;
};

class WorldRender {
    Window& window;

    void ReleaseResources() const;
    void loadSurfacesFromDirectory(const std::string& directoryPath) const;
    void GenerateWorldTexture() const;
    void GenerateWaterTextures() const;

    public:
    WorldRender(Window& window) : window(window) {};
    void GenerateTextures();

};


#endif //PREPAREWORLD_H