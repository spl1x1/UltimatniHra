//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef SPRITE_H
#define SPRITE_H
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_rect.h>

#include "../Application/dataStructures.h"

class Server;

//Sprite naming conventions for binding lookup:
// textureName_AnimationType$Direction&Frame@Variant

enum class AnimationType {
    NONE,
    IDLE,
    RUNNING,
    ATTACK,
    INTERACT,
    HURT,
    DEATH
};

enum class Direction {
    NONE,
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
    virtual void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) = 0;

    //Setters
    virtual void setVariant(int newVariant) = 0;
    virtual void setCurrentFrame(int newCurrentFrame) = 0; //For special cases

    //Getters
    virtual std::tuple<std::string,SDL_FRect*> getFrame() = 0;
    virtual RenderingContext getRenderingContext() = 0;
    [[nodiscard]] virtual int getWidth() const = 0;
    [[nodiscard]] virtual int getHeight() const = 0;
};


class SpriteAnimationBinding {
public:
    struct FrameNode {
        float x, y;
    };

    struct AnimationInfo {
        std::vector<FrameNode> frames;
        int frameCount{1};
    };
private:
    static std::unordered_map<std::string,AnimationInfo> spriteMap;
    static std::set<std::string> loadedFiles;

public:

    static void addBindings(const std::string& filePath);
    static AnimationInfo* getAnimationNode(const std::string& key);
};

class SpriteRenderingContext {
    std::unique_ptr<SDL_FRect> frameRect = std::make_unique<SDL_FRect>();
    SpriteAnimationBinding::AnimationInfo* currentAnimationNode{nullptr};

    std::string textureName;
    AnimationType activeAnimation{AnimationType::NONE};
    Direction activeDirection{Direction::NONE};
    int activeVariant{1};

    AnimationType defaultAnimation{AnimationType::NONE};
    Direction defaultDirection{Direction::NONE};
    int defaultVariant{1};

    float frameTime{0};

    float frameDuration; // 0.1 10 FPS

    int currentFrame{1};

    bool blockAnimation{false};
    bool blockRotation{false};

    int spriteWidth;
    int spriteHeight;

    //Attachers - used for texture lookup string construction
    std::string attachActiveAnimation(std::string& texture) const;
    std::string attachActiveDirection(std::string& texture) const;
    std::string attachVariantNumber(std::string& texture) const; //Attaches current variant number to texture string, made for special cases

public:
    //Methods
    void ResetAnimation(SpriteAnimationBinding::AnimationInfo* animationNode = nullptr);
    void Tick(float deltaTime);
    [[nodiscard]] std::string getTexture() const;
    [[nodiscard]] SDL_FRect* getFrameRect();

    //Getters and Setters
    void setVariant(int newVariant);
    void setCurrentFrame(int newCurrentFrame); //For special cases
    void PlayAnimation(AnimationType animationType, Direction direction, bool ForceReset); //Plays animation immediately

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    [[nodiscard]] std::string buildKey();
    std::tuple<std::string,SDL_FRect*> getFrame();

    //Constructor and Destructor
    explicit SpriteRenderingContext(const std::string& spriteJSONPath, std::string  texture, float frameDuration, int spriteWidth, int spriteHeight, Direction dir = Direction::OMNI,AnimationType anim = AnimationType::IDLE, int variant = 1);
    ~SpriteRenderingContext() = default;
};

class SpriteContext {
public:
    //Static Methods
    static std::string animationTypeToString(AnimationType type);
    static std::string directionTypeToString(Direction type);
};

#endif //SPRITE_H