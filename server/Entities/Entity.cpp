//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "Entity.h"
#include <cmath>

#include "../../MACROS.h"

Entity::~Entity() {
    delete sprite;
};

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

        if (server->getCollisionMapValue(tileX,tileY)!= 0) {
            hitbox.colliding = true;
            return ;
        }
    }
}

bool Entity::Move(float dX, float dY) {

    if (dX == 0.0f && dY == 0.0f) {
        sprite->setAnimation(IDLE);
        return true;
    };

    float relativeX = dX * speed * server->getDeltaTime();
    float relativeY = dY * speed * server->getDeltaTime();

    float newX = coordinates.x + relativeX;
    float newY = coordinates.y + relativeY;
    checkCollision(newX, newY);

    if (hitbox.colliding && !hitbox.disableCollision) return false;
    coordinates.x = newX;
    coordinates.y = newY;

    angle = std::atan2(dX, dY) * 180.0f / M_PI;
    if (angle < 0) angle += 360.0f;

    if (angle >= 45.0f && angle < 135.0f) {
        sprite->setDirection(RIGHT);
    } else if (angle >= 135.0f && angle < 225.0f) {
        sprite->setDirection(UP);
    } else if (angle >= 225.0f && angle < 315.0f) {
        sprite->setDirection(LEFT);
    } else {
        sprite->setDirection(DOWN);
    }
    sprite->setAnimation(RUNNING);

    return true;
}

Entity::Entity(int id,float maxHealth, Coordinates coordinates, EntityType type,Server *server, float speed, Sprite *sprite) {
    this->id = id;
    this->maxHealth = maxHealth;
    this->health = maxHealth;
    this->coordinates = coordinates;
    this->type = type;
    this->speed = speed;
    this->sprite = sprite;
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

