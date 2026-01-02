//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef SPRITE_H
#define SPRITE_H
#include <memory>
#include <string>
#include <SDL3/SDL_rect.h>

#include "../Application/dataStructures.h"

class Server;

enum class AnimationType {
    NONE,
    IDLE,
    RUNNING,
    ATTACK1,
    ATTACK2,
    INTERACT,
    DYING
};

enum class Direction {
    OMNI,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class ISprite {
public:
    virtual ~ISprite() = default;

    //Interface Methods
    virtual void Tick(float deltaTime) = 0;

    //Setters
    virtual void setDirection(Direction newDirection)  = 0;
    virtual void setAnimation(AnimationType newAnimation) = 0;
    virtual void setVariant(int newVariant) = 0;

    //Getters
    virtual std::tuple<std::string,SDL_FRect*> getFrame() = 0;
    virtual RenderingContext getRenderingContext() = 0;
    [[nodiscard]] virtual int getWidth() const = 0;
    [[nodiscard]] virtual int getHeight() const = 0;
};

class SpriteRenderingContext {
    std::unique_ptr<SDL_FRect> frameRect = std::make_unique<SDL_FRect>();

    std::string textureName;
    AnimationType activeAnimation = AnimationType::NONE;
    Direction direction = Direction::OMNI;

    float frameTime = 0;
    int frameCount = 0;
    float frameDuration; // 0.1 10 FPS
    int currentFrame;
    float yOffset;

    int SpriteWidth;
    int SpriteHeight;
    int FrameSpacing;
public:
    //Methods
    void Tick(float deltaTime);
    [[nodiscard]] std::string getTexture() const;
    [[nodiscard]] SDL_FRect* getFrameRect() const;

    //Getters and Setters
    void setActiveAnimation(AnimationType newAnimation);
    void setDirection(Direction newDirection);
    void setFrameCount(int newFrameCount);
    void setYOffset(float newYOffset);
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    //Attachers
    std::string attachActiveAnimation(std::string& texture) const;
    std::string attachActiveDirection(std::string& texture) const;
    std::string attachFrameNumber(std::string& texture) const; //Attaches current frame number to texture string, made for special cases

    //Constructor and Destructor
    explicit SpriteRenderingContext(std::string textureName , Direction direction = Direction::OMNI,float frameDuration = 0.1f, int frameCount =0, int spriteWidth=32, int spriteHeight=32, int frameSpacing=0, float yOffset = 0);
    ~SpriteRenderingContext() = default;
};

class SpriteContext {
public:
    //Static Methods
    static std::string animationTypeToString(AnimationType type);
    static std::string directionTypeToString(Direction type);
};

#endif //SPRITE_H