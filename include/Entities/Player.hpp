//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Entities/Entity.h"
#include "../Sprites/PlayerSprite.hpp"

class Player final : public IEntity {
    EntityRenderingComponent _entityRenderingComponent = EntityRenderingComponent(std::make_unique<PlayerSprite>());

    EntityCollisionComponent _entityCollisionComponent =
        EntityCollisionComponent(EntityCollisionComponent::HitboxData
        {
            Coordinates{41, 34}, // TOP_LEFT
            Coordinates{55, 34}, // TOP_RIGHT
            Coordinates{55, 60}, // BOTTOM_RIGHT
            Coordinates{41, 60} // BOTTOM_LEFT
        });
    EntityHealthComponent _entityHealthComponent = EntityHealthComponent(100, 100);

    EntityLogicComponent _entityLogicComponent = EntityLogicComponent();
    EntityInventoryComponent _entityInventoryComponent = EntityInventoryComponent();

    Server* _server;

public:
    //Interface methods implementation
    void Tick() override;
    RenderingContext GetRenderingContext() override;

    static void Create(Server* server, int slotId) ;
    static void Load(Server* server, int slotId);
    static void Save(Server* server);

    //Entity actions
    void Move(float dX, float dY) override;

    //Setters
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
    //Returns entity collision status
    [[nodiscard]] CollisionStatus GetCollisionStatus() const override;
    [[nodiscard]] int GetAngle() const override;
    [[nodiscard]] HitboxContext GetHitboxRenderingContext() const override;


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
