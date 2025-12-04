//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "Entity.h"
#include <cmath>

#include "../../MACROS.h"

void Entity::checkCollision(float newX, float newY) {
    hitbox.colliding = false;
    if (!collisionMap) {
        return; // No collision map defined
    }

    for (auto corner : hitbox.corners) {
        float cornerX = newX + corner.x;
        float cornerY = newY + corner.y;

        int tileX = static_cast<int>(std::floor(cornerX / 32.0f));
        int tileY = static_cast<int>(std::floor(cornerY / 32.0f));

        if (tileX < 0 || tileY < 0 || tileX >= MAPSIZE || tileY >= MAPSIZE) {
            hitbox.colliding = true;
            return ; // Out of bounds
        }

        if (collisionMap[tileX][tileY] != 0) {
            hitbox.colliding = true;
            return ;
        }
    }
}

bool Entity::Tick(float relativeX, float relativeY) {

    float newX = coordinates.x + relativeX;
    float newY = coordinates.y + relativeY;
    checkCollision(newX, newY);

    if (hitbox.colliding && !hitbox.disableCollision) return false;
    coordinates.x = newX;
    coordinates.y = newY;

    return true;
}

Entity::Entity(float maxHealth, Coordinates coordinates, EntityType type, float speed, Sprite *sprite) {
    this->maxHealth = maxHealth;
    this->health = maxHealth;
    this->coordinates = coordinates;
    this->type = type;
    this->speed = speed;
    this->sprite = sprite;

    hitbox = Hitbox{
            .corners = {
                    {0.0f, 0.0f},
                    {0.0f, 0.0f},
                    {0.0f, 0.0f},
                    {0.0f, 0.0f}
            }
    };
}

