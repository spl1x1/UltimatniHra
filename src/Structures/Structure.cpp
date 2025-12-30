//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#include "../../include/Structures/Structure.h"

#include <cmath>

#include "../../include/Application/MACROS.h"
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

void StructureRenderingComponent::renderSprite(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) const {
    if (!sprite) return;
    const auto renderingContex = sprite->getFrame();

    Rect->x = fourCorners[0].x - cameraRectangle.x;
    Rect->y = fourCorners[0].y - cameraRectangle.y;
    Rect->w = static_cast<float>(sprite->getWidth());
    Rect->h = static_cast<float>(sprite->getHeight());

    SDL_RenderTexture(&windowRenderer, textures[std::get<0>(renderingContex)], std::get<1>(renderingContex), Rect.get());
}

//StructueHitbox methods
StructureHitbox::StructureHitbox(const std::shared_ptr<Server>& server, Coordinates topLeftCorner) : server(server) {
    const Coordinates topLeft = {(std::floor(topLeftCorner.x/32))*32, (std::floor(topLeftCorner.y/32))*32};
    this->topLeftCorner = topLeft;
    this->server = server;
}


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
    for (const TrueCoordinates& point : hitboxPoints) {
        if (server->getMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP) != 0) {
            return true; // Collision detected
        }
    }
    return false; // No collision
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

bool StructureHitbox::finalize(int id) const {
    if (checkCollisionMap()) return false; // Collision detected, cannot finalize hitbox
    updateCollisionMap(id);
    return true;
}

void StructureHitbox::destroy(int id) const {
    updateCollisionMap(0, id); //Reset collision map points to 0
}
