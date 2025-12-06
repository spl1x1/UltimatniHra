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
    RUNNING,
    ATTACK1,
    ATTACK2,
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

    int SpriteWidth = 32;
    int SpriteHeight = 32;
    int FrameSpacing = 0;

    std::string activeTexture;
    std::string textureName;

    static std::string animationTypeToString(AnimationType type);
    static std::string directionTypeToString(Direction type);


    public:

    AnimationType activeAnimation = NONE;
    Direction direction = OMNI;

    virtual ~Sprite() = default;

    void changeAnimation(AnimationType newAnimation, Direction newDirection, int newFrameCount, float newFrameDuration = 0.1, bool resetFrame = false);
    void setDirection(Direction newDirection) {direction = newDirection;}
    void setAnimation(AnimationType newAnimation) {activeAnimation = newAnimation;}
    void tick(float deltaTime);

    virtual std::tuple<std::string,SDL_FRect*> getFrame();
};



#endif //SPRITE_H
