//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Chest.h"
#include "../../include/Items/ChestInventory.h"

#include "../../include/Sprites/ChestSprite.h"
#include "../../include/Sprites/Sprite.hpp"

#include <random>

#include "../../include/Entities/Entity.h"

Chest* Chest::openChest = nullptr;

Chest * Chest::GetOpenChest() { return openChest; }

structureType Chest::GetType() const {
    return structureType::CHEST;
}

int Chest::GetId() const {
    return id;
}

bool Chest::WasProperlyInitialized() {
    return initialized;
}

void Chest::Tick(const float deltaTime) {
    renderingComponent.Tick(deltaTime, this);
    if (renderingComponent.isLocked()) return;
    open ? renderingComponent.GetSprite()->PlayAnimation(AnimationType::IDLE, Direction::UP, true)
    :renderingComponent.GetSprite()->PlayAnimation(AnimationType::IDLE, Direction::DOWN, true);
}

int Chest::GetInventoryId() const {
    return -1; // Chests do not have inventories
}


RenderingContext Chest::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
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

void Chest::OpenChest() {
    SDL_Log("Opening Chest %d", id);
    renderingComponent.PlayAnimation(AnimationType::INTERACT, Direction::DOWN);
    open = true;
    openChest = this;
}

void Chest::CloseChest() {
    SDL_Log("Closing Chest %d", id);
    renderingComponent.PlayAnimation(AnimationType::INTERACT, Direction::UP);
    open = false;
    if (openChest == this) openChest = nullptr;
}

ChestStorage* Chest::getChestStorage() const {
    return chestStorage.get();
}

void Chest::Interact(IEntity *entity) {
    renderingComponent.SetLock(true);
    if (open) {
        CloseChest();
        return;
    }
    if (openChest && openChest != this)openChest->CloseChest();
    OpenChest();
}


Chest::Chest(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server)
: id(id),renderingComponent(std::make_unique<ChestSprite>()), hitboxComponent(server),
  chestStorage(std::make_unique<ChestStorage>(id)) {
    topLeftCorner = toWorldCoordinates(toTileCoordinates(topLeftCorner));
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(0, 1);
    initialized = hitboxComponent.finalize(id);
}

Chest::~Chest() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
    if (openChest && openChest == this) openChest = nullptr;
};
