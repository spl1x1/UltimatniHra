//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "../../include/Entities/Entity.h"
#include <cmath>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"



void Entity::checkCollision(float newX, float newY) {
    hitbox.colliding = false;

    for (auto corner : hitbox.corners) {
        float cornerX = newX + corner.x;
        float cornerY = newY + corner.y;

        int tileX = static_cast<int>(std::floor(cornerX / 32.0f));
        int tileY = static_cast<int>(std::floor(cornerY / 32.0f));

        if (tileX < 0 || tileY < 0 || tileX >= MAPSIZE || tileY >= MAPSIZE) {
            hitbox.colliding = true;
            return ; // Out of bounds
        }
        if (server->getMapValue_unprotected(tileX, tileY, WorldData::COLLISION_MAP) != 0) {
            hitbox.colliding = true;
            return;
        }
    }
}

bool Entity::Move(float dX, float dY, float dt) {

    if (dX == 0.0f && dY == 0.0f) {
        sprite->setAnimation(AnimationType::IDLE);
        return true;
    };

    float relativeX = dX * speed * dt;
    float relativeY = dY * speed * dt;

    float newX = coordinates.x + relativeX;
    float newY = coordinates.y + relativeY;
    checkCollision(newX, newY);

    if (hitbox.colliding && !hitbox.disableCollision) return false;
    coordinates.x = newX;
    coordinates.y = newY;

    angle = std::atan2(dX, dY) * 180.0f / M_PI;
    if (angle < 0) angle += 360.0f;

    if ((angle >= 0 && angle <= 44) || (angle >= 316 && angle <= 360)) {
        sprite->setDirection(Direction::DOWN);
    } else if (angle >= 136 && angle <= 224) {
        sprite->setDirection(Direction::UP);
    } else if (angle >= 45 && angle <= 135) {
        sprite->setDirection(Direction::RIGHT);
    } else if (angle >= 225 && angle <= 315) {
        sprite->setDirection(Direction::LEFT);
    }

    sprite->setAnimation(AnimationType::RUNNING);

    return true;
}

Entity::Entity(int id,float maxHealth, Coordinates coordinates, EntityType type, const std::shared_ptr<Server> &server, float speed, std::unique_ptr<ISprite> sprite) {
    this->id = id;
    this->maxHealth = maxHealth;
    this->health = maxHealth;
    this->coordinates = coordinates;
    this->type = type;
    this->speed = speed;
    this->sprite = std::move(sprite);
    this->server = server;

    hitbox = Hitbox{
            .corners = {
                    {0.0f, 0.0f},
                    {0.0f, 0.0f},
                    {0.0f, 0.0f},
                    {0.0f, 0.0f}
            }
    };
}

