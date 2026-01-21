//
// Created by USER on 02.01.2026.
//

#ifndef ORENODE_H
#define ORENODE_H
#include "Structure.h"
#include "../../include/Sprites/OreNodeSprite.h"

class OreNode final : public IStructure {
    int id;
    StructureRenderingComponent renderingComponent = StructureRenderingComponent(std::make_unique<OreNodeSprite>());
    StructureHitboxComponent hitboxComponent;
    bool initialized{false};
    int type{0};
    int InnerVariant{0};

public:
    //Interface implementation
    [[nodiscard]] structureType GetType() const override;
    [[nodiscard]] int GetId() const override;
    bool WasProperlyInitialized() override;
    void Tick(float deltaTime) override {};
    [[nodiscard]] int GetInventoryId() const override;;
    [[nodiscard]] RenderingContext GetRenderingContext() const override;
    [[nodiscard]] HitboxContext GetHitboxContext() override;
    [[nodiscard]] Coordinates GetCoordinates() const override;
    [[nodiscard]] int GetVariant() const override;
    [[nodiscard]] int GetInnerType() const override;

    [[nodiscard]] StructureRenderingComponent* GetRenderingComponent() override;
    [[nodiscard]] StructureHitboxComponent* GetHitboxComponent() override;
    [[nodiscard]] StructureInventoryComponent* GetInventoryComponent() override;


    void DropInventoryItems() override;
    void Interact(IEntity *entity) override{};

    //Structure specific methods

    //Constructors and Destructor
    OreNode(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, int type, int variant);
    ~OreNode() override;
};


#endif //ORENODE_H
