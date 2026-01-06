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
    OreType type;

public:
    //Interface implementation
    [[nodiscard]] structureType getType() const override;
    [[nodiscard]] int getId() const override;
    bool wasProperlyInitialized() override;
    void Tick(float deltaTime) override {};
    [[nodiscard]] RenderingContext GetRenderingContext() const override;
    [[nodiscard]] HitboxContext GetHitboxContext() override;


    //Structure specific methods

    //Constructors and Destructor
    OreDeposit(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, OreType type, int variant);
    ~OreDeposit() override;
};

#endif //ULTIMATNIHRA_OREDEPOSIT_H