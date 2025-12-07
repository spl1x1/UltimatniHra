//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "WaterSprite.hpp"

WaterSprite::WaterSprite() {
    frameDuration = 0.2f; // 5 FPS
    frameCount = 4;
    textureName = "water";
    SpriteWidth = 32;
    SpriteHeight = 32;
}

std::tuple<std::string, std::shared_ptr<SDL_FRect>> WaterSprite::getFrame() {
    return  std::tuple<std::string,std::shared_ptr<SDL_FRect>>{textureName + "_" + std::to_string(currentFrame), nullptr};
}
