//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <vector>

#include "../Server/Server.h"
#include "../Sprites/Sprite.hpp"

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

enum class TaskType{
    NOTASK,
    MOVE_TO,
    GATHER_RESOURCE,
    BUILD_STRUCTURE,
    ATTACK,
    DIE
};

struct Task{
    int taskId{} ;
    TaskType type = TaskType::NOTASK;

    Coordinates targetPosition;
    int taskType{};
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
    double angle = 0.0f;
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

    std::vector<Task> tasks;

    virtual bool Move(float dX, float dY, float dt);

    //Setters
    void SetHitbox(const Hitbox &Hitbox){ this->hitbox = Hitbox;};
    void disableCollision(const bool Switch = true){hitbox.disableCollision = Switch;};
    void setSpeed(float newSpeed){ speed = newSpeed;};
    void setSpriteOffsetX(const float newOffsetX){ offsetX = newOffsetX;}
    void setSpriteOffsetY(const float newOffsetY){ offsetY = newOffsetY;}

    //Getters
    [[nodiscard]] float getAngle() const { return angle;}
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
