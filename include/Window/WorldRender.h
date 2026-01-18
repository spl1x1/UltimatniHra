//
// Created by USER on 21.10.2025.
//

#ifndef PREPAREWORLD_H
#define PREPAREWORLD_H

#ifndef TEXTURERES
#define TEXTURERES 32
#endif

#include "Window.h"
#include <SDL3/SDL.h>
#include <string>


struct WaterSurface {
    int id;
    std::string textureName;
    SDL_Surface *surface;
    std::unique_ptr<SDL_Rect> rect;
};

class WorldRender {
    Window& window;

    void ReleaseResources() const;
    void GenerateWorldTexture() const;
    void GenerateWaterTextures() const;

    public:
    explicit WorldRender(Window& window) : window(window) {};
    void GenerateTextures() const;

};


#endif //PREPAREWORLD_H