//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef WATERSPRITE_H
#define WATERSPRITE_H
#include "Sprite.h"


class WaterSprite : public Sprite {

public:
    WaterSprite() {
        frameDuration = 0.2f; // 5 FPS
        frameCount = 4;
        textureName = "water";
    }
    std::tuple<std::string,SDL_FRect*> getFrame() override {
        auto texture = std::tuple<std::string,SDL_FRect*>{textureName + "_" + std::to_string(currentFrame), nullptr};
        return texture;
    }
};



#endif //WATERSPRITE_H
