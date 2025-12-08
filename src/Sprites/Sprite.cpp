//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/Sprite.hpp"

#include <memory>

std::string Sprite::animationTypeToString(const AnimationType type) {
    switch (type) {
        case NONE: return "NONE";
        case IDLE: return "IDLE";
        case RUNNING: return "RUNNING";
        case ATTACK1: return "ATTACK1";
        case ATTACK2: return "ATTACK2";
        case DYING: return "DYING";
        default: return "";
    }
}

std::string Sprite::directionTypeToString(const Direction type) {
    switch (type) {
        case DOWN: return "DOWN";
        case UP: return "UP";
        case LEFT: return "LEFT";
        case RIGHT: return "RIGHT";
        case OMNI: return "OMNI";
        default: return "";
    }
}

void Sprite::changeAnimation(AnimationType newAnimation, Direction newDirection, int newFrameCount,
    float newFrameDuration, bool resetFrame) {
    activeAnimation = newAnimation;
    frameCount = newFrameCount;
    frameDuration = newFrameDuration;
    if (resetFrame) {
        currentFrame = 1;
        frameTime = 0;
    }
    direction = newDirection;

    activeTexture = textureName;
    if (activeAnimation != NONE) activeTexture += "_" +animationTypeToString(activeAnimation);
    if (direction != OMNI) activeTexture += "_" + directionTypeToString(direction);
}

void Sprite::changeAnimation(AnimationType newAnimation, Direction newDirection, bool resetFrame) {
    activeAnimation = newAnimation;
    direction = newDirection;

    if (resetFrame) {
        currentFrame = 1;
        frameTime = 0;
    }

    activeTexture = textureName;
    if (activeAnimation != NONE) activeTexture += "_" +animationTypeToString(activeAnimation);
    if (direction != OMNI) activeTexture += "_" + directionTypeToString(direction);
}

void Sprite::tick(float deltaTime) {
    if (frameCount == 0) {
        return;
    }
    frameTime += deltaTime;
    if (frameTime >= frameDuration) {
        frameTime -= frameDuration;
        currentFrame++;
        if (currentFrame > frameCount) {
            currentFrame = 1;
        }
    }
}

std::tuple<std::string, std::shared_ptr<SDL_FRect>> Sprite::getFrame() {
    float x = 0;
    if (currentFrame != 1) {
        x = static_cast<float>((currentFrame - 1) * SpriteWidth + FrameSpacing);
    }
    std::shared_ptr<SDL_FRect> frameRect = std::make_shared<SDL_FRect>();
    frameRect->x =x;
    frameRect->y = yOffset;
    frameRect->w = static_cast<float>(SpriteWidth);
    frameRect->h = static_cast<float>(SpriteHeight);
    return std::make_tuple(activeTexture, frameRect);
}
