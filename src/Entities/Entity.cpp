//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "../../include/Entities/Entity.h"
#include <cmath>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"

//EntityRenderingComponent methods
void EntityRenderingComponent::Render(const SDL_Renderer* renderer, const Coordinates &entityCoordinates, const SDL_FRect &cameraRectangle, std::unordered_map<std::string, SDL_Texture*> texturePool) const {
    if (!sprite) return;

    const auto renderingContex = sprite->getFrame();

    Rect->x = entityCoordinates.x - cameraRectangle.x;
    Rect->y = entityCoordinates.y - cameraRectangle.y;
    Rect->w = static_cast<float>(sprite->getWidth());
    Rect->h = static_cast<float>(sprite->getHeight());

    SDL_RenderTexture(const_cast<SDL_Renderer*>(renderer), texturePool[std::get<0>(renderingContex)], std::get<1>(renderingContex), Rect.get());
}

void EntityRenderingComponent::Tick(const float deltaTime) const {
    if (!sprite) return;
    sprite->Tick(deltaTime);
}

void EntityRenderingComponent::SetDirectionBaseOnAngle(const int angle) const {
    if (!sprite) return;

    if ((angle >= 0 && angle <= 44) || (angle >= 316 && angle <= 360)) {
        sprite->setDirection(Direction::DOWN);
    } else if (angle >= 136 && angle <= 224) {
        sprite->setDirection(Direction::UP);
    } else if (angle >= 45 && angle <= 135) {
        sprite->setDirection(Direction::RIGHT);
    } else if (angle >= 225 && angle <= 315) {
        sprite->setDirection(Direction::LEFT);
    }
}

//EntityCollisionComponent methods
EntityCollisionComponent::HitboxData* EntityCollisionComponent::GetHitbox() {
    return  &_hitbox;
}

CollisionStatus EntityCollisionComponent::GetCollisionStatus() const {
    return CollisionStatus{
            .colliding = _hitbox.colliding,
            .collisionDisabled = _hitbox.disableCollision
    };
}

void EntityCollisionComponent::SetHitbox(const HitboxData &hitbox){
    _hitbox = hitbox;
}

void EntityCollisionComponent::disableCollision(const bool Switch){
    _hitbox.disableCollision = Switch;
}


bool EntityCollisionComponent::checkCollision(const float newX, const float newY, const std::shared_ptr<Server>& server) {
    _hitbox.colliding = false;

    for (auto corner : _hitbox.corners) {
        const float cornerX = newX + corner.x;
        const float cornerY = newY + corner.y;

        const int tileX = static_cast<int>(std::floor(cornerX / 32.0f));
        const int tileY = static_cast<int>(std::floor(cornerY / 32.0f));

        if (tileX < 0 || tileY < 0 || tileX >= MAPSIZE || tileY >= MAPSIZE) {
            _hitbox.colliding = true;
            return true; // Out of bounds
        }
        if (server->getMapValue_unprotected(tileX, tileY, WorldData::COLLISION_MAP) != 0) {
            _hitbox.colliding = true;
            return true;
        }
    }
    return false;
}

//EntityMovementComponent methods
void EntityMovementComponent::SetPathPoints(const std::vector<Coordinates> &newPathPoints) {
    pathPoints = newPathPoints;
}

std::vector<Coordinates> EntityMovementComponent::GetPathPoints() const {
    return pathPoints;
}

void EntityMovementComponent::SetTasks(const std::vector<TaskData> &newTasks) {
    tasks = newTasks;
}

std::vector<TaskData> EntityMovementComponent::GetTasks() const {
    return  tasks;
}

void EntityMovementComponent::SetAngle(const int newAngle) {
    angle = newAngle;
}

int EntityMovementComponent::GetAngle() const {
    return angle;
}

void EntityMovementComponent::SetAngleBasedOnMovement(const float dX, const float dY) {
    angle = static_cast<int>(std::floor(std::atan2(dX, dY) * 180.0f / M_PI));
    if (angle < 0) angle += 360;
}

void EntityMovementComponent::SetCoordinates(const Coordinates &newCoordinates) {
    coordinates = newCoordinates;
}

Coordinates EntityMovementComponent::GetCoordinates() const {
    return coordinates;
}

void EntityMovementComponent::MoveTo(const float targetX, const float targetY) {

    const float deltaX{targetX - currentDX};
    const float deltaY{targetY - currentDY};
    currentDX= 0.0f;
    currentDY = 0.0f;

    if (const float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY); distance > threshold) {
        currentDX = deltaX / distance;
        currentDY = deltaY / distance;
    }
}

void EntityMovementComponent::MakePath(float targetX, float targetY, const std::shared_ptr<Server> &server) {
   //pathPoints.push_back({X, Y});
   //server->getMapValue(x,y)
    //coordinates.x; coordinates.y;
}

void EntityMovementComponent::PathMovement() {
    if (pathPoints.empty()) return;
    const Coordinates targetPoint = pathPoints.front();

    MoveTo(targetPoint.x, targetPoint.y);

    if (std::abs(coordinates.x - targetPoint.x) <= threshold && std::abs(coordinates.y - targetPoint.y) <= threshold) {
        pathPoints.erase(pathPoints.begin());
    }
}


//EntityHealthComponent methods
void EntityHealthComponent::Heal(const float amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void EntityHealthComponent::TakeDamage(const float damage) {
    health -= damage;
    if (health < 0.0f) health = 0.0f;
}
void EntityHealthComponent::SetHealth(const float newHealth) {
    health = newHealth;
    if (health > maxHealth) health = maxHealth;
    if (health < 0.0f) health = 0.0f;
}
void EntityHealthComponent::SetMaxHealth(const float newMaxHealth) {
    maxHealth = newMaxHealth;
    if (health > maxHealth) health = maxHealth;
}

float EntityHealthComponent::GetHealth() const {
    return health;
}
float EntityHealthComponent::GetMaxHealth() const {
    return maxHealth;
}
bool EntityHealthComponent::isDead() const {
    return health <= 0.0f;
}

// --------------------------------------

void Entity::checkCollision(float newX, float newY) {
    hitbox.colliding = false;

    for (const auto [x, y] : hitbox.corners) {
        const float cornerX{newX + x};
        const float cornerY{newY + y};

        const int tileX{ static_cast<int>(std::floor(cornerX / 32.0f))};
        const int tileY{static_cast<int>(std::floor(cornerY / 32.0f))};

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

    angle = std::floor(std::atan2(dX, dY) * 180.0f / M_PI);
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

