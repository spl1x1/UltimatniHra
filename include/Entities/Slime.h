//
// Created by Lukáš Kaplánek on 16.01.2026.
//

#ifndef ULTIMATNIHRA_SLIME_H
#define ULTIMATNIHRA_SLIME_H

#include "Entity.h"
#include "../Sprites/SlimeSprite.h"

class Slime final: public IEntity {
    std::unique_ptr<EntityRenderingComponent> entityRenderingComponent{nullptr};
    EntityCollisionComponent entityCollisionComponent =
        EntityCollisionComponent(EntityCollisionComponent::HitboxData
        {
            Coordinates{22, 24}, // TOP_LEFT
            Coordinates{41, 24}, // TOP_RIGHT
            Coordinates{41, 41}, // BOTTOM_RIGHT
            Coordinates{22, 41} // BOTTOM_LEFT
        });
    std::unique_ptr<EntityHealthComponent> entityHealthComponent{nullptr};

    EntityLogicComponent entityLogicComponent = EntityLogicComponent();
    EntityInventoryComponent entityInventoryComponent = EntityInventoryComponent();

    Server* server;
    int id{};
    int reach{40}; //Entity reach in pixels
    float detectionRange{300.0f}; //Detection range in pixels
    float attackRange{40.0f}; //Attack range in pixels
    bool blocked{false}; //If true, player cannot perform actions (e.g., during cutscenes
    int variant{1}; //Slime variant (1 = regular, 2 = ghost)

public:
    //Interface methods implementation
    void Tick() override;
    RenderingContext GetRenderingContext() override;

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
    [[nodiscard]] float GetDetectionRange() const override;
    [[nodiscard]] float GetAttackRange() const override;
    [[nodiscard]] EntityType GetType() const override;
    [[nodiscard]] int GetVariant() const override;

    void DropItemsOnDeath() override;

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

    Slime(Server* server, const Coordinates& coordinates, int variant = 1);
};

#endif //ULTIMATNIHRA_SLIME_H