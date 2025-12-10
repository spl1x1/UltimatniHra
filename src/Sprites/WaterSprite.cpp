//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/WaterSprite.hpp"


#include <SDL3/SDL_rect.h>

std::list<std::unique_ptr<WaterSprite>> WaterSprite::instances;
std::shared_mutex WaterSprite::multitonMutex;
float WaterSprite::lastDeltaTime = 0.0f;


std::tuple<std::string, SDL_FRect*> WaterSprite::getFrame() {
    std::shared_lock lock(mutexSprite);
    std::string texture = renderingContext.getTexture();
    texture= renderingContext.attachFrameNumber(texture);
    return  std::tuple<std::string,SDL_FRect*>{texture, nullptr};
}

int WaterSprite::getInstanceCount() {
    std::shared_lock lock(multitonMutex);
    return static_cast<int>(instances.size());
}

void WaterSprite::tickInternal(float deltaTime) {
    std::lock_guard lock(mutexSprite);
    renderingContext.Tick(deltaTime);
}


void WaterSprite::Init() {
    std::lock_guard lock(multitonMutex);
    if (!instances.empty()) return;
    std::unique_ptr<WaterSprite> waterSprite = std::make_unique<WaterSprite>();
    instances.push_back(std::move(waterSprite));
}

WaterSprite* WaterSprite::getInstance(int id) {
    std::shared_lock lock(multitonMutex);
    if (id < 0 || id >= static_cast<int>(instances.size())) {
        return nullptr;
    }
    auto it = instances.begin();
    std::advance(it, id);
    return it->get();
}

void WaterSprite::Tick(float deltaTime) {
    std::lock_guard lock(multitonMutex);
    if (deltaTime == lastDeltaTime) return;
    for (auto& instance : instances) {
        instance->tickInternal(deltaTime);
    }
    lastDeltaTime = deltaTime;
}

void WaterSprite::Destroy() {
    std::lock_guard lock(multitonMutex);
    instances.clear();
}




