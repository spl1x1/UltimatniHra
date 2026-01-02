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
    return _id;
}

bool Tree::wasProperlyInitialized() {
    return initialized;
}

void Tree::Tick(float deltaTime) {
    _renderingComponent.Tick(deltaTime);
}


RenderingContext Tree::GetRenderingContext() const {
    auto context = _renderingComponent.getRenderingContext();
    context.coordinates = _hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext Tree::GetHitboxContext() {
    return _hitboxComponent.getHitboxContext();
}


Tree::Tree(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, TreeVariant variant)
    : _id(id),
      _renderingComponent(std::make_unique<TreeSprite>(), topLeftCorner), _hitboxComponent(server) {
    _renderingComponent.SetVariant(static_cast<int>(variant));
    topLeftCorner.x -= 32.0f; // Hitbox alignment
    topLeftCorner.y -= 32.0f;
    _hitboxComponent.SetTopLeftCorner(topLeftCorner);
    _hitboxComponent = StructureHitbox(server, topLeftCorner);
    _hitboxComponent.addPoint(1, 1);
    initialized = _hitboxComponent.finalize(id);
}

Tree::~Tree() {
    if (!initialized) return;
    _hitboxComponent.destroy(_id);
};