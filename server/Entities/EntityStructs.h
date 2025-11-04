//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef ENTITYSTRUCTS_H
#define ENTITYSTRUCTS_H
#include <vector>
#include "../../MACROS.h"

#include "../../render/Sprites/Sprite.h"


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

struct Entity{
    float x;
    float y;
    float speed;

    int id;
    int type;

    float health;
    float maxHealth;



    std::vector<Task> tasks;
#ifdef CLIENT
    Sprite sprite;
#endif
};

struct Player : Entity{

};



#endif //ENTITYSTRUCTS_H
