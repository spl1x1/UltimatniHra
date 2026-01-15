//
// Created by USER on 03.01.2026.
//

#include "../../include/Structures/OreNode.h"

structureType OreNode::getType() const {
    return  structureType::ORE_NODE;
}

int OreNode::getId() const {
    return  id;
}

bool OreNode::wasProperlyInitialized() {
    return initialized;
}

RenderingContext OreNode::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext OreNode::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

OreNode::OreNode(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, OreType type, const int variant)
: id(id),hitboxComponent(server), type(type) {
    renderingComponent.SetVariant(static_cast<int>(type));
    renderingComponent.GetSprite()->SetCurrentFrame(variant);
    topLeftCorner.x -= 32.0f; // Hitbox alignment
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(1, 1);
    initialized = hitboxComponent.finalize(id);
}

OreNode::~OreNode() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
};
