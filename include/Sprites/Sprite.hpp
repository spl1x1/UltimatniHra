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
    static void addBindings(const std::string& filePath);

public:
    static void Init();
    static AnimationInfo* GetAnimationNode(const std::string& key);
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
    [[nodiscard]] std::string GetTexture() const;
    [[nodiscard]] SDL_FRect* GetFrameRect();

    //Getters and Setters
    void SetVariant(int newVariant);
    void SetTexture(std::string texture);
    void SetCurrentFrame(int newCurrentFrame); //For special cases
    void PlayAnimation(AnimationType animationType, Direction direction, bool ForceReset); //Plays animation immediately
    [[nodiscard]] float GetFrameDuration() const;
    [[nodiscard]] int GetCurrentFrameCount() const;

    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] int GetCurrentFrame() const;

    [[nodiscard]] std::string BuildKey();
    std::tuple<std::string,SDL_FRect*> GetFrame();

    //Constructor and Destructor
    explicit SpriteRenderingContext(std::string  texture, float frameDuration, int spriteWidth, int spriteHeight, Direction dir = Direction::OMNI,AnimationType anim = AnimationType::IDLE, int variant = 1);
    ~SpriteRenderingContext() = default;
};

class SpriteContext {
public:
    //Static Methods
    static std::string AnimationTypeToString(AnimationType type);
    static std::string DirectionTypeToString(Direction type);
};

class ISprite {
public:
    virtual ~ISprite() = default;

    //Interface Methods
    virtual void Tick(float deltaTime) = 0;
    virtual void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) = 0;
    virtual std::tuple<float,int> GetFrameTimeAndCount() = 0;

    //Setters
    virtual void SetVariant(int newVariant) = 0;
    virtual void SetCurrentFrame(int newCurrentFrame) = 0; //For special cases

    //Getters
    virtual std::tuple<std::string,SDL_FRect*> GetFrame() = 0;
    virtual RenderingContext GetRenderingContext() = 0;
    [[nodiscard]] virtual int GetWidth() const = 0;
    [[nodiscard]] virtual int GetHeight() const = 0;
    virtual SpriteRenderingContext* GetSpriteRenderingContext() = 0;
};

#endif //SPRITE_H