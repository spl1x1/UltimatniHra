//
// Created by USER on 03.01.2026.
//

#include "../../include/Structures/OreNode.h"

#include "../../include/Items/Item.h"

structureType OreNode::GetType() const {
    return  structureType::ORE_NODE;
}

int OreNode::GetId() const {
    return  id;
}

bool OreNode::WasProperlyInitialized() {
    return initialized;
}

int OreNode::GetInventoryId() const {
    return -1;
}

RenderingContext OreNode::GetRenderingContext() const {
    auto context = renderingComponent.getRenderingContext();
    context.coordinates = hitboxComponent.getTopLeftCorner();
    return context;
}

HitboxContext OreNode::GetHitboxContext() {
    return hitboxComponent.getHitboxContext();
}

Coordinates OreNode::GetCoordinates() const {
    return hitboxComponent.getTopLeftCorner() + Coordinates{32.0f, 32.0f};
}

int OreNode::GetVariant() const {
    return type;
}

int OreNode::GetInnerType() const {
    return InnerVariant;
}

StructureRenderingComponent * OreNode::GetRenderingComponent() {
    return &renderingComponent;
}

StructureHitboxComponent * OreNode::GetHitboxComponent() {
    return &hitboxComponent;
}

StructureInventoryComponent * OreNode::GetInventoryComponent() {
    return nullptr;
}

void OreNode::DropInventoryItems() {
    switch (renderingComponent.GetVariant()) {
        case 1: // Iron
            hitboxComponent.GetServer()->AddItemToInventory(std::move(ItemFactory::createMaterial(MaterialType::IRON)));
            break;
        case 2: // Copper
            hitboxComponent.GetServer()->AddItemToInventory(std::move(ItemFactory::createMaterial(MaterialType::COPPER)));
            break;
        case 3: // Gold
            hitboxComponent.GetServer()->AddItemToInventory(std::move(ItemFactory::createMaterial(MaterialType::GOLD)));
            break;
        default:
            break;
    }
}

OreNode::OreNode(const int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, int type, const int variant)
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

OreNode::~OreNode() {
    if (!initialized) return;
    hitboxComponent.destroy(id);
};
