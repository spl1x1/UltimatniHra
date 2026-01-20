//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Chest.h"

#include "../../include/Sprites/ChestSprite.h"
#include "../../include/Sprites/Sprite.hpp"

#include <random>


structureType Chest::getType() const {
    return structureType::CHEST; // Example type, change as needed
}

int Chest::getId() const {
    return id;
}

bool Chest::wasProperlyInitialized() {
    return initialized;
}

void Chest::Tick(const float deltaTime) {
    if (renderingComponent.isLocked()) return;
    renderingComponent.Tick(deltaTime, this);
    if (renderingComponent.isLocked()) return;
    open ?renderingComponent.GetSprite()->PlayAnimation(AnimationType::IDLE, Direction::DOWN, true)
    :renderingComponent.GetSprite()->PlayAnimation(AnimationType::IDLE, Direction::UP, true);
}

int Chest::GetInventoryId() const {
    return -1; // Chests do not have inventories
}


RenderingContext Chest::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    context.coordinates -= {16.0f, 16.0f};
    return context;
}

HitboxContext Chest::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

Coordinates Chest::GetCoordinates() const {
    return hitboxComponent.getTopLeftCorner() + Coordinates{32.0f, 32.0f}; // Center of the Chest
}

int Chest::GetVariant() const {return  -1;}

int Chest::GetInnerType() const {return -1;}

StructureRenderingComponent * Chest::GetRenderingComponent() {
    return &renderingComponent;
}

StructureHitboxComponent * Chest::GetHitboxComponent() {
    return &hitboxComponent;
}

StructureInventoryComponent * Chest::GetInventoryComponent() {
    return &inventoryComponent;
}

void Chest::DropInventoryItems() {}

void Chest::Interact() {
    if (!open) {
        renderingComponent.PlayAnimation(AnimationType::INTERACT, Direction::UP);
        open = true;
    }
    else {
        renderingComponent.PlayAnimation(AnimationType::INTERACT, Direction::DOWN);
        open = false;
    }
    renderingComponent.SetLock(true);
}


Chest::Chest(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server)
: id(id),renderingComponent(std::make_unique<ChestSprite>()), hitboxComponent(server) {
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(0, 1);
    initialized = hitboxComponent.finalize(id);
}

Chest::~Chest() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
};