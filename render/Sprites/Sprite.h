//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef SPRITE_H
#define SPRITE_H
#include <string>
#include <SDL3/SDL_log.h>


class Sprite {
    protected:
    float frameTime = 0;
    float frameDuration = 0.1; // 10 FPS
    int currentFrame = 1;
    int frameCount = 0;

    std::string texturePrefix;

    public:
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
    int getCurrentFrame() const {
        return currentFrame;
    }
    int getFrameCount() const {
        return frameCount;
    }
};



#endif //SPRITE_H
