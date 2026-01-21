//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Entities/Entity.h"
#include "../Sprites/GhostSprite.h"
#include "../Sprites/PlayerSprite.hpp"
#include "../Structures/Structure.h"

struct ArmourData {
    float protection = 0.0f;
};
struct HandData {
    enum {
        NONE,
        AXE,
        PICKAXE,
        SWORD,
        PLACEABLE,
    } toolType = HandData::NONE;
    float damage = 0.0f;
};

class Player final : public IEntity {
    EntityRenderingComponent entityRenderingComponent = EntityRenderingComponent(std::make_unique<PlayerSprite>());
    EntityRenderingComponent entityRenderingComponent2 = EntityRenderingComponent(std::make_unique<GhostSprite>());

    EntityCollisionComponent entityCollisionComponent =
        EntityCollisionComponent(EntityCollisionComponent::HitboxData
        {
            Coordinates{41, 34}, // TOP_LEFT
            Coordinates{55, 34}, // TOP_RIGHT
            Coordinates{55, 60}, // BOTTOM_RIGHT
            Coordinates{41, 60} // BOTTOM_LEFT
        });
    EntityHealthComponent entityHealthComponent = EntityHealthComponent(100, 100);

    EntityLogicComponent entityLogicComponent = EntityLogicComponent();
    EntityInventoryComponent entityInventoryComponent = EntityInventoryComponent();

    Server* server;
    int id{};
    int reach{32}; //Player reach in pixels
    bool blocked{false}; //If true, player cannot perform actions (e.g., during cutscenes)
    bool isGhostMode{false}; //If true, player is in ghost mode
    bool beingRevived{false};

    int uuid{0};
    int inventoryId{0};

    HandData handData;
    ArmourData armourData;

public:
    //Interface methods implementation
    void Tick() override;
    RenderingContext GetRenderingContext() override;

    static void Create(Server* server, int slotId) ;
    static void Load(Server* server, int slotId);
    static void Save(Server* server);

    void SetGhostMode(bool enable);
    void ReviveFromGhostMode();
    [[nodiscard]] bool IsBeingRevived() const;
    [[nodiscard]] bool IsGhostMode() const;
    void SetHandData(const HandData& newHandData);
    [[nodiscard]] HandData GetHandData() const;
    void SetArmourData(const ArmourData& newArmourData);
    [[nodiscard]] ArmourData GetArmourData() const;

    //Setters
    void SetId(int newId) override;
    void SetCoordinates(const Coordinates &newCoordinates) override;
    //Sets entity angle in degrees
    void SetAngle(int newAngle) override;
    void SetSpeed(float newSpeed) override;
    //Sets current task and task data
    void SetEntityCollision(bool disable) override;
    //Event
    void AddEvent(std::unique_ptr<EntityEvent> eventData) override;
    void SetPlayerUUID(int newPlayerId);
    void SetInventoryId(int newInventoryId);

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] Coordinates GetCoordinates() const override;
    //Returns entity center coordinates
    [[nodiscard]] Coordinates GetEntityCenter() override;
    //Returns entity collision status
    [[nodiscard]] CollisionStatus GetCollisionStatus() const override;
    [[nodiscard]] int GetAngle() const override;
    [[nodiscard]] HitboxContext GetHitboxRenderingContext() const override;
    [[nodiscard]] int GetId() const override;
    [[nodiscard]] int GetReach() const override;
    [[nodiscard]] float GetSpeed() const override;
    [[nodiscard]] float GetDetectionRange() const override;;
    [[nodiscard]] float GetAttackRange() const override;;
    [[nodiscard]] EntityType GetType() const override;
    [[nodiscard]] int GetPlayerUUID() const;
    [[nodiscard]] int GetInventoryId() const;
    [[nodiscard]] int GetVariant() const override;;

    void DropItemsOnDeath() override{};

    EntityCollisionComponent* GetCollisionComponent() override;
    //Get EntityLogicComponent
    EntityLogicComponent* GetLogicComponent() override;
    //Get EntityHealthComponent
    EntityHealthComponent* GetHealthComponent() override;
    //Get EntityRenderingComponent
    EntityRenderingComponent* GetRenderingComponent() override;
    //Get EntityInventoryComponent
    EntityInventoryComponent* GetInventoryComponent() override;

    //Get server pointer
    [[nodiscard]] Server* GetServer() const override;

    Player(Server* server, const Coordinates& coordinates);
};

#endif //PLAYER_HPP
