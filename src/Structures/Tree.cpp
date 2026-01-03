//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Tree.h"

#include <random>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"
#include "../../include/Sprites/TreeSprite.hpp"

structureType Tree::getType() const {
    return structureType::TREE; // Example type, change as needed
}

int Tree::getId() const {
    return id;
}

bool Tree::wasProperlyInitialized() {
    return initialized;
}

void Tree::Tick(float deltaTime) {
    renderingComponent.Tick(deltaTime);
}


RenderingContext Tree::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext Tree::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}


Tree::Tree(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, TreeVariant variant)
: id(id),renderingComponent(std::make_unique<TreeSprite>()), hitboxComponent(server) {
    renderingComponent.SetVariant(static_cast<int>(variant));
    topLeftCorner.x -= 32.0f; // Hitbox alignment
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(1, 1);
    initialized = hitboxComponent.finalize(id);
}

Tree::~Tree() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
};