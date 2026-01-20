//
// Created by Lukáš Kaplánek on 06.01.2026.
//

#ifndef ULTIMATNIHRA_OREDEPOSIT_H
#define ULTIMATNIHRA_OREDEPOSIT_H
#include "Structure.h"
#include "../../include/Sprites/OreDepositSprite.h"

class OreDeposit final : public IStructure {
    int id;
    StructureRenderingComponent renderingComponent = StructureRenderingComponent(std::make_unique<OreDepositSprite>());
    StructureHitboxComponent hitboxComponent;
    bool initialized{false};
    int type{0};
    int InnerVariant{0};

public:
    //Interface implementation
    [[nodiscard]] structureType getType() const override;
    [[nodiscard]] int getId() const override;
    bool wasProperlyInitialized() override;
    void Tick(float deltaTime) override {};
    [[nodiscard]] int GetInventoryId() const override;
    [[nodiscard]] RenderingContext GetRenderingContext() const override;
    [[nodiscard]] HitboxContext GetHitboxContext() override;
    [[nodiscard]] Coordinates GetCoordinates() const override;
    [[nodiscard]] int GetVariant() const override;
    [[nodiscard]] int GetInnerType() const override;

    [[nodiscard]] StructureRenderingComponent* GetRenderingComponent() override;
    [[nodiscard]] StructureHitboxComponent* GetHitboxComponent() override;
    [[nodiscard]] StructureInventoryComponent* GetInventoryComponent() override;

    void DropInventoryItems() override;
    void Interact() override{};


    //Structure specific methods

    //Constructors and Destructor
    OreDeposit(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, int type, int variant);
    ~OreDeposit() override;
};

#endif //ULTIMATNIHRA_OREDEPOSIT_H