//
// Created by USER on 02.01.2026.
//

#ifndef ORENODE_H
#define ORENODE_H
#include "Structure.h"

class OreNode: public IStructure {
public:
    enum class OreType {
        IRON,
        COPPER,
        GOLD
        // more types in future
    };
private:
    int id;
    StructureRenderingComponent renderingComponent;
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
    OreNode(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server, OreType type, int variant);
    ~OreNode() override;
};


#endif //ORENODE_H
