//
// Created by USER on 09.12.2025.
//

#ifndef TREE_H
#define TREE_H

#include "Structure.h"

class Tree final : public IStructure {
    int id;
    StructureRenderingComponent renderingComponent;
    StructureHitboxComponent hitboxComponent;
    bool initialized{false};
public:
    enum class TreeVariant {
        PLAINS,
        FOREST,
        SNOW,
        NONE
    };

    int variant{0};
    int InnerVariant{0};

    //Interface implementation
    [[nodiscard]] structureType GetType() const override;
    [[nodiscard]] int GetId() const override;
    bool WasProperlyInitialized() override;
    void Tick(float deltaTime) override;
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
    void Interact(IEntity *entity) override{};

    //Constructor
    Tree(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, int innerType);
    ~Tree() override;
};


#endif //TREE_H
