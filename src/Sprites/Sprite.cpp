//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/Sprite.hpp"

#include <filesystem>
#include <utility>
#include <simdjson.h>

std::unordered_map<std::string,SpriteAnimationBinding::AnimationInfo> SpriteAnimationBinding::spriteMap{};
std::set<std::string> SpriteAnimationBinding::loadedFiles{};


void SpriteAnimationBinding::addBindings(const std::string &filePath) {
    if (!filePath.ends_with(".json")) return;
    if (loadedFiles.contains(filePath)) return;
    loadedFiles.insert(filePath);

    simdjson::ondemand::parser parser;
    const auto jsonContent{simdjson::padded_string::load(filePath)};
    auto doc{parser.iterate(jsonContent)};

    auto parseFrames = [](simdjson::ondemand::array frames, AnimationInfo& animation) {
        for (auto frameData : frames) {
            auto frame{frameData.get_object()};
            animation.frames.push_back({
            static_cast<float>(frame["x"].get_int64()),
            static_cast<float>(frame["y"].get_int64())
            });
        }
    };

    for (auto field : doc.get_object()) {
        std::string_view key = field.unescaped_key();
        simdjson::ondemand::object value = field.value().get_object();

        AnimationInfo animation;
        for (auto prop : value) {
            if (const std::string_view propKey = prop.unescaped_key(); propKey == "frames") {
                parseFrames(prop.value().get_array(), animation);
            } else if (propKey == "frameCount") {
                animation.frameCount = static_cast<int>(prop.value().get_int64());
            }
        }
        spriteMap.insert_or_assign(std::string(key), animation);
    }
}

void SpriteAnimationBinding::Init() {
    auto iterateOverDirectory= [&](const std::string& Directory)->std::vector<std::string>{
        auto dirlist = std::vector<std::string>{};
        for (const auto& entry : std::filesystem::directory_iterator(Directory)) {
            std::string fileName = entry.path().string();
            if (!entry.is_directory()) addBindings(entry.path().string());
            else dirlist.emplace_back(entry.path().string());
        }
        return dirlist;
    };

    const auto pathToJsons{"assets/jsons/animation_bindings"};
    auto iter = iterateOverDirectory(pathToJsons);
    for (const auto& entry : iter) {
        iterateOverDirectory(entry);
    }
}


SpriteAnimationBinding::AnimationInfo* SpriteAnimationBinding::GetAnimationNode(const std::string& key) {
    return spriteMap.contains(key) ? &spriteMap[key] : nullptr;
}

// SpriteRenderingContext methods
void SpriteRenderingContext::ResetAnimation(SpriteAnimationBinding::AnimationInfo* animationNode) {
    currentFrame = 1;
    frameTime = 0.0f;
    if (!animationNode) { //reset if nullptr
        activeAnimation = defaultAnimation;
        activeDirection = defaultDirection;
        currentAnimationNode = SpriteAnimationBinding::GetAnimationNode(BuildKey());
        return;
    }
    currentAnimationNode = animationNode;
}


void SpriteRenderingContext::Tick(const float deltaTime) {
    if (currentAnimationNode->frameCount == 0) return;
    frameTime += deltaTime;
    if (frameTime >= frameDuration) {
        frameTime -= frameDuration;
        currentFrame++;
        if (currentFrame > currentAnimationNode->frameCount) ResetAnimation(currentAnimationNode);
    }
}

std::string SpriteRenderingContext::attachActiveAnimation(std::string& texture) const {
    if (activeAnimation == AnimationType::NONE) return texture;
    return texture += "_" + SpriteContext::AnimationTypeToString(activeAnimation);
}

std::string SpriteRenderingContext::attachActiveDirection(std::string& texture) const {
    if (activeDirection == Direction::NONE) return texture;
    return texture += "$" + SpriteContext::DirectionTypeToString(activeDirection);
}

std::string SpriteRenderingContext::attachVariantNumber(std::string& texture) const {
    if (activeVariant <= 1) return texture;
    return texture += "@" + std::to_string(activeVariant);
}

std::string SpriteRenderingContext::BuildKey() {
    std::string key = textureName;
    if (activeAnimation == AnimationType::NONE)activeAnimation = defaultAnimation;
    attachActiveAnimation(key);
    if (activeDirection == Direction::NONE) activeDirection = defaultDirection;
    attachActiveDirection(key);
    attachVariantNumber(key);
    return key;
}

std::tuple<std::string,SDL_FRect> SpriteRenderingContext::GetFrame() {
    return {GetTexture(), GetFrameRect()};
}

void SpriteRenderingContext::PlayAnimation(const AnimationType animationType, const Direction direction, const bool ForceReset) {
    if (!ForceReset && animationType == activeAnimation && direction == activeDirection) return;
    activeAnimation = animationType;
    inReverse = false;
    this->activeDirection = direction;
    const auto key = BuildKey();
    const auto node{SpriteAnimationBinding::GetAnimationNode(key)};
    ResetAnimation(node);
}

void SpriteRenderingContext::PlayReversed() {
    inReverse = true;
}

int SpriteRenderingContext::GetVariant() const { return activeVariant; }

float SpriteRenderingContext::GetFrameDuration() const { return frameDuration; }

int SpriteRenderingContext::GetCurrentFrameCount() const { return  currentAnimationNode->frameCount; }


std::string SpriteRenderingContext::GetTexture() const {
    return textureName;
}

void SpriteRenderingContext::SetVariant(const int newVariant) {
    activeVariant = newVariant;
    ResetAnimation(SpriteAnimationBinding::GetAnimationNode(BuildKey()));
}

void SpriteRenderingContext::SetTexture(std::string texture) {
    textureName = std::move(texture);
    ResetAnimation(SpriteAnimationBinding::GetAnimationNode(BuildKey()));
}

void SpriteRenderingContext::SetCurrentFrame(int newCurrentFrame) {
    currentFrame = newCurrentFrame;
    ResetAnimation(SpriteAnimationBinding::GetAnimationNode(BuildKey()));
}



int SpriteRenderingContext::GetWidth() const {
    return spriteWidth;
}
int SpriteRenderingContext::GetHeight() const {
    return spriteHeight;
}

int SpriteRenderingContext::GetCurrentFrame() const {
    return currentFrame;
}


SDL_FRect SpriteRenderingContext::GetFrameRect() {
    if (!currentAnimationNode) currentAnimationNode = SpriteAnimationBinding::GetAnimationNode(BuildKey());
    auto frame{currentFrame};
    if (inReverse) frame = currentAnimationNode->frameCount - frame;
    SDL_FRect rect;

    rect.w = static_cast<float>(spriteWidth);
    rect.h = static_cast<float>(spriteHeight);
    rect.x = currentAnimationNode->frames.at(frame-1).x;
    rect.y = currentAnimationNode->frames.at(frame-1).y;

    return rect;
}

SpriteRenderingContext::SpriteRenderingContext(std::string  texture,const float frameDuration ,const int spriteWidth, const int spriteHeight ,const Direction dir, const AnimationType anim, const int variant):
textureName(std::move(texture)),activeAnimation(anim), activeDirection(dir), defaultVariant(variant), frameDuration(frameDuration), spriteWidth(spriteWidth), spriteHeight(spriteHeight)
{
    defaultDirection = dir;
    defaultAnimation = anim;
    currentAnimationNode = SpriteAnimationBinding::GetAnimationNode(BuildKey());
}



// SpriteContext methods
std::string SpriteContext::AnimationTypeToString(const AnimationType type) {
    switch (type) {
        case AnimationType::NONE: return "NONE";
        case AnimationType::IDLE: return "IDLE";
        case AnimationType::RUNNING: return "RUNNING";
        case AnimationType::ATTACK: return "ATTACK";
        case AnimationType::INTERACT: return "INTERACT";
        case AnimationType::HURT: return "HURT";
        case AnimationType::DEATH: return "DEATH";
        default: return "";
    }
}

std::string SpriteContext::DirectionTypeToString(const Direction type) {
    switch (type) {
        case Direction::DOWN: return "DOWN";
        case Direction::UP: return "UP";
        case Direction::LEFT: return "LEFT";
        case Direction::RIGHT: return "RIGHT";
        case Direction::OMNI: return "OMNI";
        default: return "";
    }
}
