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

void SpriteAnimationBinding::init() {
    auto iterateOverDirectory= [&](const std::string& Directory)->std::vector<std::string>{
        auto dirlist = std::vector<std::string>{};
        for (const auto& entry : std::filesystem::directory_iterator(Directory)) {
            std::string fileName = entry.path().string();
            if (!entry.is_directory()) addBindings(entry.path().string());
            else dirlist.emplace_back(entry.path().string());
        }
        return dirlist;
    };

    const auto pathToJsons{"assets/jsons"};
    auto iter = iterateOverDirectory(pathToJsons);
    for (const auto& entry : iter) {
        iterateOverDirectory(entry);
    }
}


SpriteAnimationBinding::AnimationInfo* SpriteAnimationBinding::getAnimationNode(const std::string& key) {
    return spriteMap.contains(key) ? &spriteMap[key] : nullptr;
}

// SpriteRenderingContext methods
void SpriteRenderingContext::ResetAnimation(SpriteAnimationBinding::AnimationInfo* animationNode) {
    currentFrame = 1;
    frameTime = 0.0f;
    if (!animationNode) { //reset if nullptr
        activeAnimation = defaultAnimation;
        activeDirection = defaultDirection;
        currentAnimationNode = SpriteAnimationBinding::getAnimationNode(buildKey());
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
    return texture += "_" + SpriteContext::animationTypeToString(activeAnimation);
}

std::string SpriteRenderingContext::attachActiveDirection(std::string& texture) const {
    if (activeDirection == Direction::NONE) return texture;
    return texture += "$" + SpriteContext::directionTypeToString(activeDirection);
}

std::string SpriteRenderingContext::attachVariantNumber(std::string& texture) const {
    if (activeVariant <= 1) return texture;
    return texture += "@" + std::to_string(activeVariant);
}

std::string SpriteRenderingContext::buildKey() {
    std::string key = textureName;
    if (activeAnimation == AnimationType::NONE)activeAnimation = defaultAnimation;
    attachActiveAnimation(key);
    if (activeDirection == Direction::NONE) activeDirection = defaultDirection;
    attachActiveDirection(key);
    attachVariantNumber(key);
    return key;
}

std::tuple<std::string,SDL_FRect*> SpriteRenderingContext::getFrame() {
    return {getTexture(), getFrameRect()};
}

void SpriteRenderingContext::PlayAnimation(const AnimationType animationType, const Direction direction, const bool ForceReset) {
    if (!ForceReset && animationType == activeAnimation && direction == activeDirection) return;
    activeAnimation = animationType;
    this->activeDirection = direction;
    const auto key = buildKey();
    const auto node{SpriteAnimationBinding::getAnimationNode(key)};
    ResetAnimation(node);
}


std::string SpriteRenderingContext::getTexture() const {
    return textureName;
}

void SpriteRenderingContext::setVariant(const int newVariant) {
    activeVariant = newVariant;
    ResetAnimation(SpriteAnimationBinding::getAnimationNode(buildKey()));
}

void SpriteRenderingContext::setCurrentFrame(int newCurrentFrame) {
    currentFrame = newCurrentFrame;
    ResetAnimation(SpriteAnimationBinding::getAnimationNode(buildKey()));
}



int SpriteRenderingContext::getWidth() const {
    return spriteWidth;
}
int SpriteRenderingContext::getHeight() const {
    return spriteHeight;
}


SDL_FRect* SpriteRenderingContext::getFrameRect() {
    if (!currentAnimationNode) currentAnimationNode = SpriteAnimationBinding::getAnimationNode(buildKey());
    frameRect->x = currentAnimationNode->frames.at(currentFrame-1).x;
    frameRect->y = currentAnimationNode->frames.at(currentFrame-1).y;
    return frameRect.get();
}

SpriteRenderingContext::SpriteRenderingContext(std::string  texture,const float frameDuration ,const int spriteWidth, const int spriteHeight ,const Direction dir, const AnimationType anim, const int variant):
textureName(std::move(texture)),activeAnimation(anim), activeDirection(dir), defaultVariant(variant), frameDuration(frameDuration), spriteWidth(spriteWidth), spriteHeight(spriteHeight)
{
    defaultDirection = dir;
    defaultAnimation = anim;
    currentAnimationNode = SpriteAnimationBinding::getAnimationNode(buildKey());
    frameRect->w = static_cast<float>(spriteWidth);
    frameRect->h = static_cast<float>(spriteHeight);
}



// SpriteContext methods
std::string SpriteContext::animationTypeToString(const AnimationType type) {
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
