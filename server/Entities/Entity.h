//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <vector>

#include "../../render/Sprites/Sprite.hpp"

//Defines a 2D coordinate
struct Coordinates {
    float x = 0;
    float y = 0;
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

enum TaskType{
    NOTASK,
    MOVE_TO,
    GATHER_RESOURCE,
    BUILD_STRUCTURE,
    ATTACK,
    DIE
};

class Task{
public:
    int taskId;
    TaskType type;

    Coordinates targetPosition;
    int taskType;
};

enum EntityType{
    PLAYER = 1,
    NPC = 2,
    ANIMAL = 3,
    MONSTER = 4,
    TILE_ENTITY =5,
};


class Entity {
    float offsetX= 0.0f;
    float offsetY= 0.0f;

protected:
    void checkCollision(float newX, float newY);
    float speed = 0.0f;
    int** collisionMap = nullptr;
    Hitbox hitbox {};

public:
    virtual ~Entity() = default;
    Sprite *sprite = nullptr;

    Coordinates coordinates;

    std::string name;
    int type;

    float health;
    float maxHealth;

    std::vector<Task> tasks;

    virtual bool Tick(float relativeX, float relativeY);

    //Setters
    void SetHitbox(Hitbox hitbox){ this->hitbox = hitbox;};
    void SetCollisionMap(int** map) { collisionMap = map;};
    void disableCollision(bool Switch = true){hitbox.disableCollision = Switch;};
    void setSpeed(float newSpeed){ speed = newSpeed;};
    void setSpriteOffsetX(float newOffsetX){ offsetX = newOffsetX;}
    void setSpriteOffsetY(float newOffsetY){ offsetY = newOffsetY;}

    //Getters
    [[nodiscard]] float GetSpeed() const { return speed;}
    [[nodiscard]] Hitbox* GetHitbox() { return &hitbox;}
    [[nodiscard]] bool collisionDisabled() const {return hitbox.disableCollision;}
    [[nodiscard]] bool isColliding() const {return hitbox.colliding;}
    //Returns sprite center
    [[nodiscard]] Coordinates getTrueCoordinates() const { return Coordinates{coordinates.x + offsetX, coordinates.y + offsetY};}

    Entity(float maxHealth, float x, float y, EntityType type, float speed=0.0f, Sprite *sprite = nullptr);

};


#endif //ENTITY_H
