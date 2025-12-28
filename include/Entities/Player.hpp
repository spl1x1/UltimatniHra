//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Entities/Entity.h"
#include "../Sprites/PlayerSprite.hpp"

class Player final : public IEntity {
    EntityRenderingComponent _entityRenderingComponent;
    EntityCollisionComponent _entityCollisionComponent;
    EntityLogicComponent _entityLogicComponent;
    EntityHealthComponent _entityHealthComponent;
    EntityInventoryComponent _entityInventoryComponent;

    Server* _server;

public:
    //Interface methods implementation
    void Tick() override;
    void Render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) override;

    static void Create(Server* server) ;
    static void Load(Server* server);

    //Entity actions
    void Move(float dX, float dY) override;
    void HandleTask(TaskData data) override;

    //Setters
    void SetCoordinates(const Coordinates &newCoordinates) override;
    //Sets entity angle in degrees
    void SetAngle(int newAngle) override;
    void SetSpeed(float newSpeed) override;
    //Sets current task and task data
    void SetTask(int index) override;
    void RemoveTask(int index) override;
    void SetEntityCollision(bool disable) override;
    //Event
    void AddEvent(const EventData &eventData) override;

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] Coordinates GetCoordinates() const override;
    //Returns entity collision status
    [[nodiscard]] CollisionStatus GetCollisionStatus() const override;
    [[nodiscard]] int GetAngle() const override;


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
