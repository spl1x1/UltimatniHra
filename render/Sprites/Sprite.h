//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef SPRITE_H
#define SPRITE_H
#include <string>
#include <SDL3/SDL_rect.h>

enum AnimationType {
    NONE,
    IDLE,
    WALKING,
    RUNNING,
    ATTACKING,
    DYING
};

enum Direction {
    OMNI,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Sprite {
    protected:
    float frameTime = 0;
    float frameDuration = 0.1; // 10 FPS
    int currentFrame = 1;
    int frameCount = 0;
    float yOffset = 0;
    std::string activeTexture;
    std::string textureName;
    AnimationType activeAnimation = NONE;
    Direction direction = OMNI;

    public:

    void changeAnimation(AnimationType newAnimation, Direction newDirection, int newFrameCount, float newFrameDuration = 0.1) {
        activeAnimation = newAnimation;
        frameCount = newFrameCount;
        frameDuration = newFrameDuration;
        currentFrame = 1;
        frameTime = 0;
        direction = newDirection;

        activeTexture = textureName;
        if (activeAnimation != NONE) activeTexture += "_" +std::to_string(activeAnimation);
        if (direction != OMNI) activeTexture += "_" +std::to_string(direction);
    };

    void tick(float deltaTime) {
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
    virtual std::tuple<std::string,SDL_FRect*> getFrame() {
        float x = (static_cast<float>(currentFrame) - 1) * 32;
        return {activeTexture, new SDL_FRect{x,yOffset,32,32} };
    }
};



#endif //SPRITE_H
