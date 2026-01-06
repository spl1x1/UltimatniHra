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
        SNOW,
        FOREST,
        NONE
    };
    //Interface implementation
    [[nodiscard]] structureType getType() const override;
    [[nodiscard]] int getId() const override;
    bool wasProperlyInitialized() override;
    void Tick(float deltaTime) override;
    [[nodiscard]] RenderingContext GetRenderingContext() const override;
    [[nodiscard]] HitboxContext GetHitboxContext() override;

    //Constructor
    Tree(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, TreeVariant variant);
    ~Tree() override;
};


#endif //TREE_H
