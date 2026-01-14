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

std::unique_ptr<EventBindings> EventBindings::instance = nullptr;

//EntityRenderingComponent methods
void EntityRenderingComponent::Tick(const float deltaTime) const {
    if (!sprite) return;
    sprite->Tick(deltaTime);
}

Coordinates EntityRenderingComponent::CalculateCenterOffset(IEntity& entity) {
    if (offset.x != -1.0f && offset.y != -1.0f) {
        return offset;
    }
    const auto hitbox = entity.GetCollisionComponent()->GetHitbox()->corners;

    const auto hitboxCenter = Coordinates{
        (hitbox[0].x + hitbox[1].x) / 2.0f,
        (hitbox[0].y + hitbox[2].y) / 2.0f
    };
    return offset = hitboxCenter;
}

RenderingContext EntityRenderingComponent::GetRenderingContext() const {
    if (!sprite) return RenderingContext{};

    auto renderingContext = sprite->getRenderingContext();
    return renderingContext;
}

Direction EntityRenderingComponent::GetDirectionBaseOnAngle(const int angle) {
    if ((angle >= 0 && angle <= 44) || (angle >= 316 && angle <= 360)) {
        return Direction::DOWN;
    }
    if (angle >= 136 && angle <= 224) {
        return Direction::UP;
    }
    if (angle >= 45 && angle <= 135) {
        return Direction::RIGHT;
    }
    if (angle >= 225 && angle <= 315) {
        return Direction::LEFT;
    }
    return Direction::NONE;
}

void EntityRenderingComponent::PlayAnimation(const AnimationType animation, const Direction direction, const bool ForceReset) const {
    if (!sprite) return;
    sprite->PlayAnimation(animation, direction, ForceReset);
}

EntityRenderingComponent::EntityRenderingComponent(std::unique_ptr<ISprite> sprite) :sprite(std::move(sprite)) {
    rect = std::make_unique<SDL_FRect>();
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

bool EntityLogicComponent::IsInterrupted() const {
    return lockTime > 0.0f;
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
    auto isInInterrupts= [&](const int index)->bool {
        return std::ranges::any_of(interruptedEvents,
                                   [&] (const EntityEvent::Type interruptedEvent) {
                                       return interruptedEvent== events.at(index)->GetType();
                                   });
    }; //Check if event type is in interruptedEvents

    auto process = [&](const int index) {
        const auto event = events.at(index).get();
        event->SetDeltaTime(dt);
        if (event->validate()) EventBindings::Callback(&entity, event);
    }; //Process event at index

    auto updateLockTime = [&](const float deltaTime) {
        if (lockTime > 0.0f) {
            lockTime -= deltaTime;
            if (lockTime < 0.0f) lockTime = 0.0f;
        }
    };

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
    updateLockTime(dt);
}

void EntityLogicComponent::AddEvent(std::unique_ptr<EntityEvent> eventData) {
    events.push_back(std::move(eventData));
}

bool EntityLogicComponent::Move(const float deltaTime, const float dX,const float dY, EntityCollisionComponent &collisionComponent, const Server* server) {
    if (lockTime > 0.0f) return false;
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

void EntityLogicComponent::MoveTo(const float deltaTime, const float targetX, const float targetY, IEntity* entity) {
    if (lockTime > 0.0f) return;
    const auto adjustment = entity->GetRenderingComponent()->CalculateCenterOffset(*entity);
    float diffX{targetX - coordinates.x - adjustment.x};
    float diffY{targetY - coordinates.y - adjustment.y};
    const float distance{std::sqrt(diffX * diffX + diffY * diffY)};

    if (distance <= threshold)
        return; //Already at target

    diffX /= distance;
    diffY /= distance;

    if (!Move(deltaTime, diffX, diffY, *entity->GetCollisionComponent(), entity->GetServer())) return;
    queueUpEvents.emplace_back(Event_MoveTo::Create(targetX,targetY));
}

void EntityLogicComponent::PerformAttack(IEntity* entity, const int attackType, const int damage) {
    lockTime = 0.5f; //Example lock time for attack
    const auto angle{entity->GetLogicComponent()->GetAngle()};
    const auto reach{entity->GetReach()};
    const auto adjustment = entity->GetRenderingComponent()->CalculateCenterOffset(*entity);
    const auto coordinates = entity->GetLogicComponent()->GetCoordinates();
    const auto radianAngle = static_cast<float>(angle * M_PI / 180.0f);

    const float attackX = coordinates.x + adjustment.x + std::sin(radianAngle) * reach;
    const float attackY = coordinates.y + adjustment.y + std::cos(radianAngle) * reach;

    entity->GetServer()->applyDamageAt_unprotected(damage,{attackX, attackY} );

    const auto renderingComponent = entity->GetRenderingComponent();
    const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(angle);
    renderingComponent->PlayAnimation(AnimationType::ATTACK, direction, true);
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

void EventBindings::Callback(IEntity* entity, const EntityEvent* eventData) {
    instance->bindings.at(eventData->GetType())(entity, eventData);
}

void EventBindings::InitializeBindings() {
    instance = std::make_unique<EventBindings>();
    instance->bindings = std::unordered_map<EntityEvent::Type, std::function<void(IEntity* entity, const EntityEvent* eventData)>>();
    instance->bindings.insert_or_assign(EntityEvent::Type::INTERRUPT,  [](IEntity* entity, const EntityEvent *e) {
        auto logicComponent = entity->GetLogicComponent();
        logicComponent->interrupted = true;
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::INTERRUPT_SPECIFIC, [](IEntity* entity, const EntityEvent* e) {
        const auto data =  dynamic_cast<const Event_InterruptSpecific*>(e);
        entity->GetLogicComponent()->interruptedEvents.emplace(data->eventToInterrupt);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::CHANGE_COLLISION, [](IEntity* entity, const EntityEvent *e) {
        entity->GetCollisionComponent()->SwitchCollision();
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::MOVE, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_Move*>(e);
        entity->GetLogicComponent()->Move(data->GetDeltaTime(), data->dX, data->dY, *entity->GetCollisionComponent(), entity->GetServer());

    });
    instance->bindings.insert_or_assign(EntityEvent::Type::MOVE_TO, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_MoveTo*>(e);
        entity->GetLogicComponent()->MoveTo(data->GetDeltaTime(), data->targetX, data->targetY, entity);

    });
    instance->bindings.insert_or_assign(EntityEvent::Type::CLICK_MOVE, [](IEntity* entity, const EntityEvent *e) {
       const auto data = dynamic_cast<const Event_ClickMove*>(e);
        entity->GetLogicComponent()->interrupted = true;
        entity->GetLogicComponent()->MoveTo(data->GetDeltaTime(), data->targetX, data->targetY, entity);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::HEAL, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_Heal*>(e);
        entity->GetHealthComponent()->Heal(data->amount);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::DAMAGE, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_Damage*>(e);
        entity->GetHealthComponent()->TakeDamage(data->amount);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::ATTACK, [](IEntity* entity, const EntityEvent *e) {
        const auto server{entity->GetServer()};
        const auto data =  dynamic_cast<const Event_Attack*>(e);
        server->applyDamageAt_unprotected(data->damage, entity->GetLogicComponent()->GetCoordinates());
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::CLICK_ATTACK, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_ClickAttack*>(e);
        entity->GetLogicComponent()->PerformAttack(entity, data->attackType, data->damage);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::SET_ANGLE, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_SetAngle*>(e);
        entity->GetLogicComponent()->SetAngle(data->angle);
    });

}

