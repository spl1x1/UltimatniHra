//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Tree.h"

#include <random>

#include "../../include/Items/Item.h"
#include "../../include/Sprites/Sprite.hpp"
#include "../../include/Sprites/TreeSprite.hpp"

structureType Tree::GetType() const {
    return structureType::TREE;
}

int Tree::GetId() const {
    return id;
}

bool Tree::WasProperlyInitialized() {
    return initialized;
}

void Tree::Tick(const float deltaTime) {
    renderingComponent.Tick(deltaTime, this);
}

int Tree::GetInventoryId() const {
    return -1; // Trees do not have inventories
}


RenderingContext Tree::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext Tree::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

Coordinates Tree::GetCoordinates() const {
    return hitboxComponent.getTopLeftCorner() + Coordinates{32.0f, 32.0f}; // Center of the tree
}

int Tree::GetVariant() const {
    return InnerVariant; // Example variant, change as needed
}

int Tree::GetInnerType() const {
    return variant;
}

StructureRenderingComponent * Tree::GetRenderingComponent() {
    return  &renderingComponent;
}

StructureHitboxComponent * Tree::GetHitboxComponent() {
    return &hitboxComponent;
}

StructureInventoryComponent * Tree::GetInventoryComponent() {
    return nullptr;
}

void Tree::DropInventoryItems() {
    hitboxComponent.GetServer()->AddItemToInventory(std::move(ItemFactory::createMaterial(MaterialType::WOOD)));
}


Tree::Tree(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, int innerType)
: id(id),renderingComponent(std::make_unique<TreeSprite>()), hitboxComponent(server) {
    renderingComponent.SetVariant(innerType);
    topLeftCorner.x -= 32.0f; // Hitbox alignment
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(1, 1);
    initialized = hitboxComponent.finalize(id);
    InnerVariant = innerType;
}

Tree::~Tree() {
    if (!initialized) return;
    hitboxComponent.destroy(id);

};