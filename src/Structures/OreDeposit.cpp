//
// Created by Lukáš Kaplánek on 06.01.2026.
//

#include "../../include/Structures/OreDeposit.h"

structureType OreDeposit::getType() const {
    return structureType::ORE_DEPOSIT;
}

int OreDeposit::getId() const {
    return  id;
}

bool OreDeposit::wasProperlyInitialized() {
    return initialized;
}

RenderingContext OreDeposit::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext OreDeposit::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

OreDeposit::OreDeposit(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, OreType type, const int variant)
: id(id),hitboxComponent(server), type(type) {
    renderingComponent.SetVariant(static_cast<int>(type));
    renderingComponent.GetSprite()->SetCurrentFrame(variant);
    topLeftCorner.x -= 32.0f; // Hitbox alignment
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(1, 1);
    initialized = hitboxComponent.finalize(id);
}

OreDeposit::~OreDeposit() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
};