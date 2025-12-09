//
// Created by USER on 09.12.2025.
//

#include "../../include/Sprites/TreeSprite.hpp"

TreeSprite::TreeSprite() {
    frameDuration = 0.2f; // 5 FPS
    frameCount = 16;
    textureName = "tree";
    SpriteWidth = 96;
    SpriteHeight = 64;
}

std::tuple<std::string, std::shared_ptr<SDL_FRect>> TreeSprite::getFrame() {
    return  std::tuple<std::string,std::shared_ptr<SDL_FRect>>{textureName + "_" + std::to_string(currentFrame), nullptr};
}
