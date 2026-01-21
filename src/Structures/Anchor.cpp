//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Anchor.h"

#include "../../include/Sprites/AnchorSprite.h"
#include "../../include/Sprites/Sprite.hpp"

#include "../../include/Entities/Entity.h"
#include "../../include/Entities/Player.hpp"


structureType Anchor::GetType() const {
    return structureType::RESPAWN_ANCHOR;
}

int Anchor::GetId() const {
    return id;
}

bool Anchor::WasProperlyInitialized() {
    return initialized;
}

void Anchor::Tick(const float deltaTime) {
    renderingComponent.Tick(deltaTime, this);
    if (renderingComponent.isLocked())  return;
    renderingComponent.PlayAnimation(AnimationType::IDLE,Direction::DOWN);
    if (!interactingEntity) return;
    if (interactingEntity->GetType() != EntityType::PLAYER)  return;
    dynamic_cast<Player*>(interactingEntity)->ReviveFromGhostMode();
    interactingEntity = nullptr;
    beingUsed = false;
}

int Anchor::GetInventoryId() const {
    return -1; // Anchors do not have inventories
}


RenderingContext Anchor::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext Anchor::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

Coordinates Anchor::GetCoordinates() const {
    return hitboxComponent.getTopLeftCorner() + Coordinates{32.0f, 32.0f}; // Center of the Anchor
}

int Anchor::GetVariant() const {return  -1;}

int Anchor::GetInnerType() const {return -1;}

StructureRenderingComponent * Anchor::GetRenderingComponent() {
    return &renderingComponent;
}

StructureHitboxComponent * Anchor::GetHitboxComponent() {
    return &hitboxComponent;
}

StructureInventoryComponent * Anchor::GetInventoryComponent() {
    return &inventoryComponent;
}


void Anchor::Interact(IEntity *entity) {
    if (entity->GetType() != EntityType::PLAYER) return;
    if (!dynamic_cast<Player*>(entity)->IsGhostMode()) return;
    renderingComponent.SetLock(true);
    interactingEntity=entity;
    beingUsed = true;
    renderingComponent.PlayAnimation(AnimationType::INTERACT,Direction::UP);
}


Anchor::Anchor(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server)
: id(id),renderingComponent(std::make_unique<AnchorSprite>()), hitboxComponent(server) {
    topLeftCorner = toWorldCoordinates(toTileCoordinates(topLeftCorner));
    topLeftCorner.y -= 64.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(0, 2);
    initialized = hitboxComponent.finalize(id);
    if (initialized ) server->respawnPoints.push_back(topLeftCorner + Coordinates{16.0f, 80.0f});
}

Anchor::~Anchor() {
    if (initialized) hitboxComponent.destroy(id);
};