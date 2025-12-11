//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <any>
#include <vector>

#include "../Server/Server.h"
#include "../Sprites/Sprite.hpp"

enum class Event{
    // data = float dX, float dY
    MOVE,
    // data = float targetX, float targetY
    MOVE_TO,
    // data = attackType
    ATTACK,
    // data = int structureType, Coordinates position
    PLACE,
    // data = Coordinates position
    INTERACT,
    // data = std::vector<Item> items
    INVENTORY
};

struct EventData {
    Event type;
    /*
     Parameters for the event
     first parameter should be used for delta time if needed
     */
    std::vector<std::any> params;
};

enum class TaskType{
    NOTASK,
    MOVE_TO,
    GATHER_RESOURCE,
    BUILD_STRUCTURE,
    ATTACK,
    DIE
};

struct TaskData {
    TaskType type;
    /*
     Parameters for the task
     */
    std::vector<std::any> params;
    //Task status
    bool completed = false;
};

struct EntityData {
    int angle = 0;
    float speed = 0.0f;
    Coordinates coordinates;
    TaskData currentTask;
};

class IEntity {
public:
    //Interface methods
    virtual ~IEntity() = 0;
    virtual void Tick(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Create() = 0;
    virtual void Load() = 0;

    //Entity actions
    virtual void Move(float dX, float dY) = 0;
    virtual void MoveTo(float targetX, float targetY) = 0;
    virtual void handleAction(EventData data) = 0;

    //Setters
    virtual void SetCoordinates(const Coordinates &newCoordinates) = 0;
    //Sets entity angle in degrees
    virtual void SetAngle(float newAngle) = 0;
    virtual void SetSpeed(float newSpeed) = 0;
    //Sets current task and task data
    virtual void SetTask(TaskData data) = 0;

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] virtual Coordinates GetCoordinates() const = 0;
    //Returns entity collision status
    [[nodiscard]] virtual CollisionStatus getCollisionStatus() const = 0;
    [[nodiscard]] virtual EntityData GetEntityData() const = 0;
    //Returns current task and task data
    [[nodiscard]] virtual TaskData GetTask() const = 0;
};

//Defines 4 hitbox corners, relative to sprite
struct Hitbox {
    Coordinates corners[4];
    bool disableCollision = false;
    bool colliding = false;
};

enum HitboxCorners{
    TOP_LEFT =0,
    TOP_RIGHT =1,
    BOTTOM_LEFT =2,
    BOTTOM_RIGHT =3
};


enum class EntityType{
    PLAYER,
    NPC,
    ANIMAL,
    MONSTER,
    TILE_ENTITY
};


class Entity {
    float offsetX= 0.0f;
    float offsetY= 0.0f;
    int angle = 0;
    std::shared_ptr<Server> server;

protected:
    void checkCollision(float newX, float newY);
    float speed = 0.0f;
    Hitbox hitbox {};

public:
    int id;
    virtual ~Entity() = default;
    std::unique_ptr<ISprite> sprite = nullptr;

    Coordinates coordinates;

    std::string name;
    EntityType type;

    float health;
    float maxHealth;


    virtual bool Move(float dX, float dY, float dt);

    //Setters
    void SetHitbox(const Hitbox &Hitbox){ this->hitbox = Hitbox;};
    void disableCollision(const bool Switch = true){hitbox.disableCollision = Switch;};
    void setSpeed(float newSpeed){ speed = newSpeed;};
    void setSpriteOffsetX(const float newOffsetX){ offsetX = newOffsetX;}
    void setSpriteOffsetY(const float newOffsetY){ offsetY = newOffsetY;}

    //Getters
    [[nodiscard]] int getAngle() const { return angle;}
    [[nodiscard]] float GetSpeed() const { return speed;}
    [[nodiscard]] Hitbox* GetHitbox() { return &hitbox;}
    [[nodiscard]] bool collisionDisabled() const {return hitbox.disableCollision;}
    [[nodiscard]] bool isColliding() const {return hitbox.colliding;}
    //Returns sprite center
    [[nodiscard]] Coordinates getTrueCoordinates() const { return Coordinates{coordinates.x + offsetX, coordinates.y + offsetY};}

    // Base Entity class, all entities inherit from this
    Entity(int id, float maxHealth, Coordinates coordinates, EntityType type, const std::shared_ptr<Server> &server, float speed=0.0f, std::unique_ptr<ISprite> sprite = nullptr);
};


#endif //ENTITY_H
