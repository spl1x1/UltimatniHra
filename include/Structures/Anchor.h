//
// Created by USER on 09.12.2025.
//

#ifndef ANCHOR_H
#define ANCHOR_H

#include "Structure.h"

class Anchor final : public IStructure {
    int id;
    StructureRenderingComponent renderingComponent;
    StructureHitboxComponent hitboxComponent;
    StructureInventoryComponent inventoryComponent;
    bool initialized{false};
    bool beingUsed{false};
    IEntity* interactingEntity{nullptr};

public:
    //Interface implementation
    [[nodiscard]] structureType GetType() const override;
    [[nodiscard]] int GetId() const override;
    bool WasProperlyInitialized() override;
    void Tick(float deltaTime) override;
    [[nodiscard]] int GetInventoryId() const override;
    [[nodiscard]] RenderingContext GetRenderingContext() const override;
    [[nodiscard]] HitboxContext GetHitboxContext() override;
    [[nodiscard]] Coordinates GetCoordinates() const override;
    [[nodiscard]] int GetVariant() const override;;
    [[nodiscard]] int GetInnerType() const override;;

    [[nodiscard]] StructureRenderingComponent* GetRenderingComponent() override;
    [[nodiscard]] StructureHitboxComponent* GetHitboxComponent() override;
    [[nodiscard]] StructureInventoryComponent* GetInventoryComponent() override;


    void DropInventoryItems() override{};
    void Interact(IEntity *entity) override;

    //Constructor
    Anchor(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server);
    ~Anchor() override;
};


#endif //ANCHOR_H
