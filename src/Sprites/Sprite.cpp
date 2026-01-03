//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/Sprite.hpp"

#include <memory>
#include <utility>

// SpriteRenderingContext methods
void SpriteRenderingContext::Tick(float deltaTime) {
        if (frameCount == 0) return;
        frameTime += deltaTime;
        if (frameTime >= frameDuration) {
            frameTime -= frameDuration;
            currentFrame++;
            if (currentFrame > frameCount) {
                currentFrame = 1;
            }
        }
}

std::string SpriteRenderingContext::attachActiveAnimation(std::string& texture) const {
    if (activeAnimation == AnimationType::NONE) return texture;
    return texture += "_" + SpriteContext::animationTypeToString(activeAnimation);
}

std::string SpriteRenderingContext::attachActiveDirection(std::string& texture) const {
    if (direction == Direction::OMNI) return texture;
    return texture += "_" + SpriteContext::directionTypeToString(direction);
}

std::string SpriteRenderingContext::attachFrameNumber(std::string& texture) const {
    return texture += "_" + std::to_string(currentFrame);
}

std::string SpriteRenderingContext::getTexture() const {
    return textureName;
}


void SpriteRenderingContext::setFrameCount(int newFrameCount) {
    frameCount = newFrameCount;
    if (currentFrame > frameCount) {
        currentFrame = 1;
    }
}

void SpriteRenderingContext::setVariant(int newVariant) {
    if (newVariant < 1 || newVariant > variantCount) return;
    currentVariant = newVariant;
}

void SpriteRenderingContext::setCurrentFrame(int newCurrentFrame) {
    if (newCurrentFrame < 1 || newCurrentFrame > frameCount) newCurrentFrame = 1;
    currentFrame = newCurrentFrame;
}

void SpriteRenderingContext::setActiveAnimation(AnimationType newAnimation){
    if (activeAnimation != newAnimation) {
        activeAnimation = newAnimation;
        currentFrame = 1;
        frameTime = 0.0f;
    }
}

void SpriteRenderingContext::setDirection(Direction newDirection){
    direction = newDirection;
}

int SpriteRenderingContext::getWidth() const {
    return spriteWidth;
}
int SpriteRenderingContext::getHeight() const {
    return spriteHeight;
}


SDL_FRect* SpriteRenderingContext::getFrameRect() const {
    float x = 0;
    float y = 0;
    if (currentFrame > 1) {
        x = static_cast<float>((currentFrame - 1) * spriteWidth) + xOffset;
    }
    if (currentVariant > 1) {
        y += static_cast<float>((currentVariant - 1) * spriteHeight) + yOffset;
    }
    frameRect->x =x;
    frameRect->y = y;
    frameRect->w = static_cast<float>(spriteWidth);
    frameRect->h = static_cast<float>(spriteHeight);
    return frameRect.get();
}

SpriteRenderingContext::SpriteRenderingContext(std::string textureName, const Direction direction , const float frameDuration, const int frameCount, const int spriteWidth, const int spriteHeight, const int variants, const int currentVariant, const float xOffset, const float yOffset)
: textureName(std::move(textureName)), direction(direction), frameCount(frameCount), frameDuration(frameDuration), currentFrame(1), yOffset(yOffset), spriteWidth(spriteWidth), spriteHeight(spriteHeight), variantCount(variants), currentVariant(currentVariant), xOffset(xOffset){}



// SpriteContext methods
std::string SpriteContext::animationTypeToString(const AnimationType type) {
    switch (type) {
        case AnimationType::NONE: return "NONE";
        case AnimationType::IDLE: return "IDLE";
        case AnimationType::RUNNING: return "RUNNING";
        case AnimationType::ATTACK1: return "ATTACK1";
        case AnimationType::ATTACK2: return "ATTACK2";
        case AnimationType::INTERACT: return "INTERACT";
        case AnimationType::DYING: return "DYING";
        default: return "";
    }
}

std::string SpriteContext::directionTypeToString(const Direction type) {
    switch (type) {
        case Direction::DOWN: return "DOWN";
        case Direction::UP: return "UP";
        case Direction::LEFT: return "LEFT";
        case Direction::RIGHT: return "RIGHT";
        case Direction::OMNI: return "OMNI";
        default: return "";
    }
}
