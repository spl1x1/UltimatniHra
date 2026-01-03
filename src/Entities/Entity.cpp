//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "../../include/Entities/Entity.h"
#include <cmath>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"
#include "../../include/Entities/EntityEvent.h"
#include <queue>
#include <algorithm>

//EntityRenderingComponent methods
void EntityRenderingComponent::Tick(const float deltaTime) const {
    if (!_sprite) return;
    _sprite->Tick(deltaTime);
}

RenderingContext EntityRenderingComponent::GetRenderingContext() const {
    if (!_sprite) return RenderingContext{};

    auto renderingContext = _sprite->getRenderingContext();
    return renderingContext;
}

void EntityRenderingComponent::SetDirectionBaseOnAngle(const int angle) const {
    if (!_sprite) return;

    if ((angle >= 0 && angle <= 44) || (angle >= 316 && angle <= 360)) {
        _sprite->setDirection(Direction::DOWN);
    } else if (angle >= 136 && angle <= 224) {
        _sprite->setDirection(Direction::UP);
    } else if (angle >= 45 && angle <= 135) {
        _sprite->setDirection(Direction::RIGHT);
    } else if (angle >= 225 && angle <= 315) {
        _sprite->setDirection(Direction::LEFT);
    }
}

void EntityRenderingComponent::SetAnimation(const AnimationType animation) const {
    if (!_sprite) return;
    _sprite->setAnimation(animation);
}

void EntityRenderingComponent::SetSprite(std::unique_ptr<ISprite> sprite) {
    _sprite = std::move(_sprite);
}

EntityRenderingComponent::EntityRenderingComponent(std::unique_ptr<ISprite> sprite) :_sprite(std::move(sprite)) {
    _rect = std::make_unique<SDL_FRect>();
}

//EntityCollisionComponent methods
EntityCollisionComponent::HitboxData* EntityCollisionComponent::GetHitbox() {
    return  &_hitbox;
}

CollisionStatus EntityCollisionComponent::GetCollisionStatus() const {
    return CollisionStatus{
            .colliding = _hitbox.colliding,
            .collisionDisabled = _hitbox.disabledCollision
    };
}

HitboxContext EntityCollisionComponent::GetHitboxContext() const {
    HitboxContext context{};
    //To be implemented
    context.corners = std::vector(_hitbox.corners, _hitbox.corners + 4);

    if (_hitbox.colliding) context.r = 255;
    else context.g = 255;
    if (_hitbox.disabledCollision) context.b = 255;

    return context;
}

void EntityCollisionComponent::SetHitbox(const HitboxData &hitbox){
    _hitbox = hitbox;
}

void EntityCollisionComponent::DisableCollision(const bool Switch){
    _hitbox.disabledCollision = Switch;
}

void EntityCollisionComponent::SwitchCollision(){
    _hitbox.disabledCollision = !_hitbox.disabledCollision;
}

bool EntityCollisionComponent::CheckCollision(const float newX, const float newY, const Server* server) {
    _hitbox.colliding = false;

    auto evaluatePoint = [&, newX, newY](const Coordinates corner){;
        const int tileX{static_cast<int>(std::floor((corner.x + newX) / 32.0f))};
        const int tileY{static_cast<int>(std::floor((corner.y + newY) / 32.0f))};

        if (tileX < 0 || tileY < 0
            || tileX >= MAPSIZE
            || tileY >= MAPSIZE
            || server->getMapValue_unprotected(tileX, tileY, WorldData::COLLISION_MAP) != 0) {
            _hitbox.colliding = true;
        }
    };

    std::ranges::for_each(_hitbox.corners, evaluatePoint);
    return _hitbox.colliding;
}

bool EntityCollisionComponent::CheckCollisionAt(const float newX, const float newY, const Server* server) const{
    bool result{false};

    auto evaluatePoint = [&, newX, newY](const Coordinates corner){;
        const int tileX{static_cast<int>(std::floor((corner.x + newX) / 32.0f))};
        const int tileY{static_cast<int>(std::floor((corner.y + newY) / 32.0f))};

        if (tileX < 0 || tileY < 0
            || tileX >= MAPSIZE
            || tileY >= MAPSIZE
            || server->getMapValue_unprotected(tileX, tileY, WorldData::COLLISION_MAP) != 0) {
            result = true;
            }
    };

    std::ranges::for_each(_hitbox.corners, evaluatePoint);
    return result;
}

//EntityMovementComponent methods


void EntityLogicComponent::SetAngle(const int newAngle) {
    angle = newAngle;
}

int EntityLogicComponent::GetAngle() const {
    return angle;
}

void EntityLogicComponent::SetSpeed(float newSpeed) {
    if (newSpeed < 0.0f) return;
    speed = newSpeed;
}

float EntityLogicComponent::GetSpeed() const {
    return speed;
}


void EntityLogicComponent::SetAngleBasedOnMovement(const float dX, const float dY) {
    angle = static_cast<int>(std::floor(std::atan2(dX, dY) * 180.0f / M_PI));
    if (angle < 0) angle += 360;
}

void EntityLogicComponent::SetCoordinates(const Coordinates &newCoordinates) {
    coordinates = newCoordinates;
}

Coordinates EntityLogicComponent::GetCoordinates() const {
    return coordinates;
}


void EntityLogicComponent::Tick(const Server* server, IEntity& entity) {
    const auto dt = server->getDeltaTime_unprotected();
    auto isInInterrupts= [&](int index)->bool {
        return std::ranges::any_of(interruptedEvents,
                                   [&] (EntityEvent::Type interruptedEvent) {
                                       return interruptedEvent== events.at(index)->GetType();
                                   });
    }; //Check if event type is in interruptedEvents

    auto process = [&](int index) {
        events.at(index)->SetDeltaTime(dt);
        HandleEvent(server, entity, index);
    }; //Process event at index

    for (auto &e : queueUpEvents) {
        events.emplace_back(std::move(e));
    }
    queueUpEvents.clear();

    for (auto eventIndex{0}; eventIndex < events.size(); ++eventIndex) {
        if (!interrupted && !isInInterrupts(eventIndex)) process(eventIndex);
    }
    events.clear();
    interruptedEvents.clear();
    interrupted = false;
}

void EntityLogicComponent::AddEvent(std::unique_ptr<EntityEvent> eventData) {
    events.push_back(std::move(eventData));
}

EntityLogicComponent::EntityLogicComponent(const Coordinates &coordinates) : coordinates(coordinates) {
    //TODO::Temp to be used later
    //Register MOVE_TO script binding - not used, different approach implemented
    /* RegisterScriptBinding("MOVE_TO", {
        .scriptName = "MOVE_TO",
        .function = EntityScripts::MoveToScript
    }); */
}


bool EntityLogicComponent::Move(const float deltaTime, const float dX,const float dY, EntityCollisionComponent &collisionComponent, const Server* server) {
    const float newX{coordinates.x + dX * speed* deltaTime};
    const float newY{coordinates.y + dY * speed * deltaTime};

    collisionComponent.CheckCollision(newX, newY, server);
    const EntityCollisionComponent::HitboxData hitbox{*collisionComponent.GetHitbox()};

    if (hitbox.colliding && !hitbox.disabledCollision) return false;
    this->coordinates.x = newX;
    this->coordinates.y = newY;
    SetAngleBasedOnMovement(dX, dY);
    return true;
}

void EntityLogicComponent::MoveTo(const float deltaTime, const float targetX, const float targetY, EntityCollisionComponent &collisionComponent, const Server* server) {
    const float diffX{targetX - coordinates.x};
    const float diffY{targetY - coordinates.y};
    const float distance{std::sqrt(diffX * diffX + diffY * diffY)};

    if (distance <= threshold)
        return; //Already at target

    const float directionX{diffX / distance};
    const float directionY{diffY / distance};

    if (!Move(deltaTime, directionX, directionY, collisionComponent, server)) return;
    queueUpEvents.emplace_back(Event_MoveTo::Create(targetX,targetY));
}

void EntityLogicComponent::HandleEvent(const Server* server, IEntity &entity, int eventIndex) {
    const auto *e = events.at(eventIndex).get();
    if (!e->validate()) return;
    if (auto type = e->GetType(); type == EntityEvent::Type::INTERRUPT) {
        interrupted = true;
    }
    else if (type == EntityEvent::Type::INTERRUPT_SPECIFIC) {
        auto data =  dynamic_cast<const Event_InterruptSpecific*>(e);
        interruptedEvents.emplace(data->eventToInterrupt);
    }
    else if (type == EntityEvent::Type::MOVE) {
        auto data =  dynamic_cast<const Event_Move*>(e);
        Move(data->GetDeltaTime(), data->dX, data->dY, *entity.GetCollisionComponent(), server);
    }
    else if (type == EntityEvent::Type::CLICK_MOVE) {
        auto data = dynamic_cast<const Event_ClickMove*>(e);
        interrupted = true;
        MoveTo(data->GetDeltaTime(), data->targetX, data->targetY, *entity.GetCollisionComponent(), server);
        return;
    }
    else if (type == EntityEvent::Type::MOVE_TO) {
        auto data = dynamic_cast<const Event_MoveTo*>(e);
        MoveTo(data->GetDeltaTime(), data->targetX, data->targetY, *entity.GetCollisionComponent(), server);
    }
    else if (type == EntityEvent::Type::CHANGE_COLLISION) {
        entity.GetCollisionComponent()->SwitchCollision();
    }
    else if (type == EntityEvent::Type::DAMAGE) {
        auto data = dynamic_cast<const Event_Damage*>(e);
        entity.GetHealthComponent()->TakeDamage(data->amount);
    }
    else if (type == EntityEvent::Type::HEAL) {
        auto data = dynamic_cast<const Event_Heal*>(e);
        entity.GetHealthComponent()->Heal(data->amount);
    }
}

//EntityHealthComponent methods
void EntityHealthComponent::Heal(const int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void EntityHealthComponent::TakeDamage(const int damage) {
    health -= damage;
    if (health < 0) health = 0;
}
void EntityHealthComponent::SetHealth(const int newHealth) {
    health = newHealth;
    if (health > maxHealth) health = maxHealth;
    if (health < 0) health = 0;
}
void EntityHealthComponent::SetMaxHealth(const int newMaxHealth) {
    maxHealth = newMaxHealth;
    if (health > maxHealth) health = maxHealth;
}

int EntityHealthComponent::GetHealth() const {
    return health;
}
int EntityHealthComponent::GetMaxHealth() const {
    return maxHealth;
}
bool EntityHealthComponent::isDead() const {
    return health <= 0;
}

