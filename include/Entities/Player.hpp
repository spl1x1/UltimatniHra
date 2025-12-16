//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Entities/Entity.h"
#include "../Sprites/PlayerSprite.hpp"


class Player;

enum class PlayerEvents{
    MOVE,
    ATTACK,
    PLACE,
    INTERACT,
    INVENTORY
};

struct PlayerEvent {
    PlayerEvents type;
    float data1;
    float data2;
    float deltaTime;
};


class PlayerNew final : public IEntity {
    EntityRenderingComponent _entityRenderingComponent;
    EntityCollisionComponent _entityCollisionComponent;
    EntityLogicComponent _entityLogicComponent;
    EntityHealthComponent _entityHealthComponent;
    EntityInventoryComponent _entityInventoryComponent;

    std::shared_ptr<Server> _server;

public:
    //Interface methods implementation
    void Tick() override;
    void Render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) override;
    void Create() override;
    void Load() override;

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

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] Coordinates GetCoordinates() const override;
    //Returns entity collision status
    [[nodiscard]] CollisionStatus GetCollisionStatus() const override;
    [[nodiscard]] int GetAngle() const override;
    //Returns current task and task data
    [[nodiscard]] TaskData GetTask() const override;
    //Returns task queue
    [[nodiscard]] std::vector<TaskData> GetTasks() const override;
    //Returns event queue
    [[nodiscard]] std::vector<EventData> GetEvents() const override;

    PlayerNew(std::shared_ptr<Server> server, const Coordinates& coordinates);
};

class Player final : public Entity {
    using Entity::Move;
public:
    void handleEvent(PlayerEvent e); //TODO: implement

    // Constructors
    Player(int id, float maxHealth, Coordinates coordinates, const std::shared_ptr<Server>& server ,float speed);

    // Initializes the player character for client, should be called only once
    static void ClientInit(const std::shared_ptr<Server>& server);
};



#endif //PLAYER_HPP
