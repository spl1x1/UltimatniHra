//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <vector>

#include "../../render/Sprites/Sprite.hpp"

struct Hitbox {
    float offsetX;
    float offsetY;
    float width;
    float height;
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

    float targetX;
    float targetY;
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
protected:

    [[nodiscard]] bool checkCollision(float newX, float newY) const;
    float speed = 0.0f;
    int** collisionMap = nullptr;
    Hitbox hitbox {0.0f,0.0f,32.0f,32.0f};

public:
    virtual ~Entity() = default;
    Sprite *sprite = nullptr;

    float x;
    float y;

    std::string name;
    int type;

    float health;
    float maxHealth;

    std::vector<Task> tasks;

    virtual void Tick(float relativeX, float relativeY);

    //Setters
    void SetHitbox(Hitbox hitbox){ this->hitbox = hitbox;};
    void SetCollisionMap(int** map) { collisionMap = map;};

    //Getters
    [[nodiscard]] float GetSpeed() const { return speed;}

    Entity(float maxHealth, float x, float y, EntityType type, float speed=0.0f, Sprite *sprite = nullptr);

};


#endif //ENTITY_H
