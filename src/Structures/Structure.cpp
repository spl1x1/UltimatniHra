//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#include "../../include/Structures/Structure.h"

#include <cmath>

#include "../../include/Sprites/Sprite.hpp"

//StructureRenderingComponent methods

StructureRenderingComponent::StructureRenderingComponent(std::unique_ptr<ISprite> sprite) : sprite(std::move(sprite)){}

void StructureRenderingComponent::Tick(float deltaTime) const {
    if (!sprite) return;
    sprite->Tick(deltaTime);
}

void StructureRenderingComponent::SetVariant(const int variant) const {
    if (!sprite) return;
    sprite->SetVariant(variant);
}

int StructureRenderingComponent::GetVariant() const {
    return sprite ? sprite->GetSpriteRenderingContext()->GetVariant() : 1;
}

ISprite* StructureRenderingComponent::GetSprite() const {
    return sprite.get();
}

std::string StructureRenderingComponent::TypeToString(const structureType type) {
    switch (type) {
        case structureType::TREE:
            return "Tree";
        case structureType::ORE_NODE:
            return "Ore Node";
        case structureType::ORE_DEPOSIT:
            return "Ore Deposit";
        default:
            return "Unknown";
    }
}

RenderingContext StructureRenderingComponent::getRenderingContext() const {
    if (!sprite) return RenderingContext{};

    auto renderingContext = sprite->GetRenderingContext();
    return renderingContext;
}

//StructueHitbox methods
StructureHitboxComponent::StructureHitboxComponent(const std::shared_ptr<Server>& server, Coordinates topLeftCorner) : server(server) {
    const Coordinates topLeft = {(std::floor(topLeftCorner.x/32))*32, (std::floor(topLeftCorner.y/32))*32};
    this->topLeftCorner = topLeft;
}

StructureHitboxComponent::StructureHitboxComponent(const std::shared_ptr<Server>& server) : server(server) {}

int InventoryComponent::GetInventoryId() const {
    return inventoryId;
}

void StructureHitboxComponent::updateCollisionMap(int value, int checkValue) const {
    for (const TrueCoordinates& point : hitboxPoints) {
        if (checkValue != -2) {
            if (server->GetMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP) != checkValue) {
                continue; // Skip updating this point if it's already occupied
            }
        }
        server->SetMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP, value);
    }
}

bool StructureHitboxComponent::checkCollisionMap() const {
     return std::ranges::any_of(hitboxPoints, [this](const TrueCoordinates& point) {
        return server->GetMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP) != 0;
    });
}

Coordinates StructureHitboxComponent::getTopLeftCorner() const {
    return topLeftCorner;
}

void StructureHitboxComponent::SetTopLeftCorner(const Coordinates topLeftCorner) {
    this->topLeftCorner = topLeftCorner;
}

void StructureHitboxComponent::addRowOfPoints(int posX, int posY, int length) {
    for (int i = 0; i < length; i++) {
        addPoint(posX + i, posY);
    }
}

void StructureHitboxComponent::addColumnOfPoints(int posX, int posY, int length) {
    for (int i = 0; i < length; i++) {
        addPoint(posX, posY + i);
    }
}

void StructureHitboxComponent::addPoint(int posX, int posY){
    TrueCoordinates point{};
    point.x = static_cast<int>(topLeftCorner.x)/32 + posX;
    point.y = static_cast<int>(topLeftCorner.y)/32 + posY;
    hitboxPoints.push_back(point);
}

HitboxContext StructureHitboxComponent::getHitboxContext() {
    if (!hitboxContext.corners.empty()) {
        return hitboxContext; // Return cached context if already computed
    }

    HitboxContext context{};
    context.coordinates = topLeftCorner;
    context.r = 255;
    context.g = 255;
    context.b = 0;
    context.a = 255;

    if (hitboxPoints.empty()) return context;

    auto it = hitboxPoints.begin();
    int minX{it->x};
    int minY{it->y};
    int maxX{it->x};
    int maxY{it->y};

    ++it;
    for (;it != hitboxPoints.end(); ++it) {
        if (it->x < minX) minX = it->x;
        if (it->y < minY) minY = it->y;
        if (it->x > maxX) maxX = it->x;
        if (it->y > maxY) maxY = it->y;
    }

    const auto left{static_cast<float>(minX * 32 - static_cast<int>(topLeftCorner.x))};
    const auto top{ static_cast<float>(minY * 32 - static_cast<int>(topLeftCorner.y))};
    const auto right{static_cast<float>((maxX + 1) * 32 - static_cast<int>(topLeftCorner.x))};
    const auto bottom{static_cast<float>((maxY + 1) * 32 - static_cast<int>(topLeftCorner.y))};

    context.corners.push_back({left, top});
    context.corners.push_back({right, top});
    context.corners.push_back({right, bottom});
    context.corners.push_back({left, bottom});

    hitboxContext = context; // Cache the computed context
    return context;
}

bool StructureHitboxComponent::finalize(int id) const {
    if (checkCollisionMap()) return false; // Collision detected, cannot finalize hitbox
    updateCollisionMap(id);
    return true;
}

void StructureHitboxComponent::destroy(int id) const {
    updateCollisionMap(0, id); //Reset collision map points to 0
}
