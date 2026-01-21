//
// Created by USER on 09.12.2025.
//

#ifndef CHEST_H
#define CHEST_H

#include "Structure.h"
#include <memory>

class ChestStorage;

class Chest final : public IStructure {
    int id;
    StructureRenderingComponent renderingComponent;
    StructureHitboxComponent hitboxComponent;
    StructureInventoryComponent inventoryComponent;
    bool initialized{false};
    bool open{false};

    std::unique_ptr<ChestStorage> chestStorage;

    static Chest* openChest;

public:
    static Chest* GetOpenChest();

    //Interface implementation
    [[nodiscard]] structureType getType() const override;
    [[nodiscard]] int getId() const override;
    bool wasProperlyInitialized() override;
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

    void OpenChest();
    void CloseChest();

    // Chest storage access
    [[nodiscard]] ChestStorage* getChestStorage() const;

    void DropInventoryItems() override;;
    void Interact(IEntity *entity) override;

    //Constructor
    Chest(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server);
    ~Chest() override;
};


#endif //CHEST_H
