//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef SPRITE_H
#define SPRITE_H
#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_rect.h>

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
    //Interace Methods
    virtual ~ISprite() = 0;
    virtual void Tick(float deltaTime) = 0;

    //Setters
    virtual void setDirection(Direction newDirection)  = 0;
    virtual void setAnimation(AnimationType newAnimation) = 0;

    //Getters
    virtual std::tuple<std::string,SDL_FRect*> getFrame();
    [[nodiscard]] virtual int getWidth() const = 0;
    [[nodiscard]] virtual int getHeight() const = 0;
};

class SpriteRenderingContext {
    std::unique_ptr<SDL_FRect> frameRect = std::make_unique<SDL_FRect>();

    std::string textureName;
    AnimationType activeAnimation = AnimationType::NONE;
    Direction direction = Direction::OMNI;

    float frameTime = 0;
    float frameDuration = 0.1; // 10 FPS
    int currentFrame = 1;
    int frameCount = 0;
    float yOffset = 0;

    int SpriteWidth = 32;
    int SpriteHeight = 32;
    int FrameSpacing = 0;
public:
    void Tick(float deltaTime);
    [[nodiscard]] std::string getTexture() const;
    [[nodiscard]] SDL_FRect* getFrameRect() const;

    //Attachers
    void attachActiveAnimation(std::string* texture) const;
    void attachActiveDirection(std::string* texture) const;
};

class SpriteContext {
public:
    static std::string animationTypeToString(AnimationType type);
    static std::string directionTypeToString(Direction type);
};

class Sprite {
    std::unique_ptr<SDL_FRect> frameRect = std::make_unique<SDL_FRect>();
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
    AnimationType activeAnimation = AnimationType::NONE;
    Direction direction = Direction::OMNI;

    public:

    //Methods
    static std::string animationTypeToString(AnimationType type);
    static std::string directionTypeToString(Direction type);
    void changeAnimation(AnimationType newAnimation, Direction newDirection, int newFrameCount, float newFrameDuration = 0.1, bool resetFrame = false);
    void changeAnimation(AnimationType newAnimation, Direction newDirection, bool resetFrame = false) ;
    void tick(float deltaTime);

    //Setters
    void setDirection(Direction newDirection) {changeAnimation(activeAnimation, newDirection);}
    void setAnimation(AnimationType newAnimation) {changeAnimation(newAnimation, direction);}

    //Getters
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    virtual std::tuple<std::string,SDL_FRect*> getFrame();

    //Debug
    [[nodiscard]] Direction getDirection() const {return direction;}
    [[nodiscard]] AnimationType getActiveAnimation() const {return activeAnimation;}

    //Constructors and Destructors
    virtual ~Sprite() = default;
};



#endif //SPRITE_H
