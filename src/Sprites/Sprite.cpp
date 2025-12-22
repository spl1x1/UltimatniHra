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
    return SpriteWidth;
}
int SpriteRenderingContext::getHeight() const {
    return SpriteHeight;
}


SDL_FRect* SpriteRenderingContext::getFrameRect() const {
    float x = 0;
    if (currentFrame != 1) {
        x = static_cast<float>((currentFrame - 1) * SpriteWidth + FrameSpacing);
    }
    frameRect->x =x;
    frameRect->y = yOffset;
    frameRect->w = static_cast<float>(SpriteWidth);
    frameRect->h = static_cast<float>(SpriteHeight);
    return frameRect.get();
}

SpriteRenderingContext::SpriteRenderingContext(std::string textureName, Direction direction ,float frameDuration, int frameCount, int spriteWidth, int spriteHeight, int frameSpacing, float yOffset)
: textureName(std::move(textureName)), direction(direction), frameCount(frameCount), frameDuration(frameDuration), currentFrame(1), yOffset(yOffset), SpriteWidth(spriteWidth), SpriteHeight(spriteHeight), FrameSpacing(frameSpacing){}



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
