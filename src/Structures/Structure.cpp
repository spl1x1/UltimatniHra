//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#include "../../include/Structures/Structure.h"

#include <cmath>

#include "../../include/Sprites/Sprite.hpp"

//StructureRenderingComponent methods

StructureRenderingComponent::StructureRenderingComponent(std::unique_ptr<ISprite> sprite, Coordinates topLeft) : sprite(std::move(sprite)){
    Coordinates topLeftCorner = {(std::floor(topLeft.x/32))*32, (std::floor(topLeft.y/32))*32};
    auto width = static_cast<float>(this->sprite->getWidth());
    auto height = static_cast<float>(this->sprite->getHeight());
    this->fourCorners[0] = topLeftCorner; //Top-left
    this->fourCorners[1] = {topLeftCorner.x + width, topLeftCorner.y}; //Top-right
    this->fourCorners[2] = {topLeftCorner.x, topLeftCorner.y + height}; //Bottom-left
    this->fourCorners[3] = {topLeftCorner.x + width, topLeftCorner.y + height}; //Bottom-right
}

void StructureRenderingComponent::Tick(float deltaTime) const {
    if (!sprite) return;
    sprite->Tick(deltaTime);
}

void StructureRenderingComponent::SetVariant(int variant) const {
    if (!sprite) return;
    sprite->setVariant(variant);
}

RenderingContext StructureRenderingComponent::getRenderingContext() const {
    if (!sprite) return RenderingContext{};

    auto renderingContext = sprite->getRenderingContext();
    return renderingContext;
}

//StructueHitbox methods
StructureHitbox::StructureHitbox(const std::shared_ptr<Server>& server, Coordinates topLeftCorner) : server(server) {
    const Coordinates topLeft = {(std::floor(topLeftCorner.x/32))*32, (std::floor(topLeftCorner.y/32))*32};
    this->topLeftCorner = topLeft;
}

StructureHitbox::StructureHitbox(const std::shared_ptr<Server>& server) : server(server) {}

void StructureHitbox::updateCollisionMap(int value, int checkValue) const {
    for (const TrueCoordinates& point : hitboxPoints) {
        if (checkValue != -2) {
            if (server->getMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP) != checkValue) {
                continue; // Skip updating this point if it's already occupied
            }
        }
        server->setMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP, value);
    }
}

bool StructureHitbox::checkCollisionMap() const {
     return std::ranges::any_of(hitboxPoints, [this](const TrueCoordinates& point) {
        return server->getMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP) != 0;
    });
}

Coordinates StructureHitbox::getTopLeftCorner() const {
    return topLeftCorner;
}

void StructureHitbox::SetTopLeftCorner(const Coordinates topLeftCorner) {
    this->topLeftCorner = topLeftCorner;
}

void StructureHitbox::addRowOfPoints(int posX, int posY, int length) {
    for (int i = 0; i < length; i++) {
        addPoint(posX + i, posY);
    }
}

void StructureHitbox::addColumnOfPoints(int posX, int posY, int length) {
    for (int i = 0; i < length; i++) {
        addPoint(posX, posY + i);
    }
}

void StructureHitbox::addPoint(int posX, int posY){
    TrueCoordinates point{};
    point.x = static_cast<int>(topLeftCorner.x)/32 + posX;
    point.y = static_cast<int>(topLeftCorner.y)/32 + posY;
    hitboxPoints.push_back(point);
}

HitboxContext StructureHitbox::getHitboxContext() {
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

bool StructureHitbox::finalize(int id) const {
    if (checkCollisionMap()) return false; // Collision detected, cannot finalize hitbox
    updateCollisionMap(id);
    return true;
}

void StructureHitbox::destroy(int id) const {
    updateCollisionMap(0, id); //Reset collision map points to 0
}
