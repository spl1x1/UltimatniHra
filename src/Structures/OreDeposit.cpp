//
// Created by Lukáš Kaplánek on 06.01.2026.
//

#include "../../include/Structures/OreDeposit.h"

#include <variant>

structureType OreDeposit::GetType() const {
    return structureType::ORE_DEPOSIT;
}

int OreDeposit::GetId() const {
    return  id;
}

bool OreDeposit::WasProperlyInitialized() {
    return initialized;
}

int OreDeposit::GetInventoryId() const {
    return -1;
}

RenderingContext OreDeposit::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext OreDeposit::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

Coordinates OreDeposit::GetCoordinates() const {
    return hitboxComponent.getTopLeftCorner() + Coordinates{32.0f, 32.0f};
}

int OreDeposit::GetVariant() const {
    return type ;
}

int OreDeposit::GetInnerType() const {
    return InnerVariant;
}

StructureRenderingComponent * OreDeposit::GetRenderingComponent() {
    return &renderingComponent;
}

StructureHitboxComponent * OreDeposit::GetHitboxComponent() {
    return &hitboxComponent;
}

StructureInventoryComponent * OreDeposit::GetInventoryComponent() {
    return nullptr;
}

void OreDeposit::DropInventoryItems() {
}

OreDeposit::OreDeposit(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, int type, const int variant)
: id(id),hitboxComponent(server), type(type) {

    renderingComponent.SetVariant(variant);
    renderingComponent.GetSprite()->SetCurrentFrame(type);
    topLeftCorner.x -= 32.0f; // Hitbox alignment
    topLeftCorner.y -= 32.0f;
    hitboxComponent.SetTopLeftCorner(topLeftCorner);
    hitboxComponent.addPoint(1, 1);
    initialized = hitboxComponent.finalize(id);
    InnerVariant = variant;
}

OreDeposit::~OreDeposit() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
};