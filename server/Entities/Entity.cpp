//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "Entity.h"
#include "../../MACROS.h"

bool Entity::checkCollision(float newX, float newY) const {
    /*
    if (!collisionMap) {
        return true; // No collision map defined
    }
    int tileX = static_cast<int>(std::floor(newX / 32.0f));
    int tileY = static_cast<int>(std::floor(newY / 32.0f));

    if (tileX < 0 || tileY < 0 || tileX >= MAPSIZE || tileY >= MAPSIZE) {
        return false; // Out of bounds
    }

    if (collisionMap[tileX][tileY] != 0) {
        return false;
    }
    */
    return true;
}

void Entity::Tick(float relativeX, float relativeY) {

    float newX = x + relativeX;
    float newY = y + relativeY;

    if (checkCollision(newX, newY)) {
        x = newX;
        y = newY;
    }
}

Entity::Entity(float maxHealth, float x, float y, EntityType type, float speed, Sprite *sprite) {
    this->maxHealth = maxHealth;
    this->health = maxHealth;
    this->x = x;
    this->y = y;
    this->type = type;
    this->speed = speed;
    this->sprite = sprite;
}

