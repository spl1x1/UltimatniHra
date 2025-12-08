//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#include "Structure.h"
#include "../../render/Sprites/Sprite.hpp"

//StructureRenderingComponent methods

StructureRenderingComponent::StructureRenderingComponent(std::unique_ptr<Sprite> sprite) {
    this->sprite = std::move(sprite);
}

//StructueHitbox methods
StructureHitbox::StructureHitbox(const std::shared_ptr<Server> &server, Coordinates topLeftCorner) {
    this->topLeftCorner = topLeftCorner;
    this->server = server;
}

StructureHitbox::~StructureHitbox() {
    updateCollisionMap(0);
}

void StructureHitbox::updateCollisionMap(int value) const {
    for (const TrueCoordinates& point : hitboxPoints) {
        server->setMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP, value);
    }
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

void StructureHitbox::finalize() {
    updateCollisionMap(1);
}

//Structure methods
Structure::Structure(int id, structureType type) {
    this->id = id;
    this->type = type;
}