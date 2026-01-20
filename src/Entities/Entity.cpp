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

#include "../../include/Entities/Player.hpp"

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

std::string EntityRenderingComponent::TypeToString(EntityType type) {
    switch (type) {
        case EntityType::PLAYER:
            return "PLAYER";
        case EntityType::SLIME:
            return "SLIME";
        default:
            return "UNKNOWN";
    }
}

EntityType EntityRenderingComponent::StringToType(const std::string &type) {
    if (type == "PLAYER") {
        return EntityType::PLAYER;
    }
    if (type == "SLIME") {
        return EntityType::SLIME;
    }
    return EntityType::UNKNOWN;
}

RenderingContext EntityRenderingComponent::GetRenderingContext() const {
    if (!sprite) return RenderingContext{};

    auto renderingContext = sprite->GetRenderingContext();
    return renderingContext;
}

std::tuple<float, int> EntityRenderingComponent::GetFrameTimeAndCount() const {
     return sprite ? sprite->GetFrameTimeAndCount() : std::make_tuple(0.0f, 0);
}

ISprite * EntityRenderingComponent::GetSprite() const {
    return sprite.get();
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

void EntityRenderingComponent::PlayAnimation(const AnimationType animation, const Direction direction, const int variant, const bool ForceReset) const {
    if (!sprite) return;
    sprite->SetVariant(variant);
    sprite->PlayAnimation(animation, direction, ForceReset);
}

void EntityRenderingComponent::PlayReversedAnimation(const AnimationType animation, const Direction direction, const int variant,
    const bool ForceReset) const {
    if (!sprite) return;
    sprite->SetVariant(variant);
    sprite->PlayAnimation(animation, direction, ForceReset);
    sprite->GetSpriteRenderingContext()->PlayReversed();
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
            || server->GetMapValue_unprotected(tileX, tileY, WorldData::COLLISION_MAP) != 0) {
            _hitbox.colliding = true;
            }
    };
    std::ranges::for_each(_hitbox.corners, evaluatePoint);
    return _hitbox.colliding;
}

bool EntityCollisionComponent::CheckCollisionAt(const float newX, const float newY,IEntity& entity){
    bool result{false};

    auto evaluatePoint = [&, newX, newY](const Coordinates corner){;
        const int tileX{static_cast<int>(std::floor((corner.x + newX) / 32.0f))};
        const int tileY{static_cast<int>(std::floor((corner.y + newY) / 32.0f))};

        if (tileX < 0 || tileY < 0
            || tileX >= MAPSIZE
            || tileY >= MAPSIZE
            || entity.GetServer()->GetMapValue_unprotected(tileX, tileY, WorldData::COLLISION_MAP) != 0) {
            result = true;
            }
    };
    const auto hitbox{entity.GetCollisionComponent()->GetHitbox()};
    std::ranges::for_each(hitbox->corners, evaluatePoint);
    return result;
}

bool EntityCollisionComponent::CheckCollisionAtTile(int tileX, int tileY, IEntity &entity) {
    const auto hitbox{entity.GetCollisionComponent()->GetHitbox()};

    auto evaluatePoint = [&, tileX, tileY](const Coordinates corner){;
        const int cornerTileX{static_cast<int>(std::floor((corner.x + entity.GetLogicComponent()->GetCoordinates().x) / 32.0f))};
        const int cornerTileY{static_cast<int>(std::floor((corner.y + entity.GetLogicComponent()->GetCoordinates().y) / 32.0f))};

        if (cornerTileX == tileX && cornerTileY == tileY) {
            return true;
        }
        return false;
    };

    return std::ranges::any_of(hitbox->corners, evaluatePoint);
}

bool EntityCollisionComponent::CheckPoint(const Coordinates coordinates, IEntity& entity ) const {
    const auto entityPosition = entity.GetLogicComponent()->GetCoordinates();
    const Coordinates Points[2] ={
        {_hitbox.corners[0].x + entityPosition.x, _hitbox.corners[0].y + entityPosition.y}, // TOP_LEFT
        {_hitbox.corners[2].x + entityPosition.x, _hitbox.corners[2].y + entityPosition.y}  // BOTTOM_LEFT
    };
    if ((coordinates.x >= Points[0].x && coordinates.x <= Points[1].x)
        && (coordinates.y >= Points[0].y && coordinates.y <= Points[1].y))
        return true; //Point is inside hitbox
    return false;
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

void EntityLogicComponent::SetLock() {
    lock = true;
}

bool EntityLogicComponent::IsLocked() const { return lock; }

void EntityLogicComponent::SetInterrupted(const bool newInterrupted) {
    interrupted = newInterrupted;
}

bool EntityLogicComponent::IsInterrupted() const {
    return lock || interrupted;
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

Coordinates EntityLogicComponent::GetLastMoveDirection() const {
    return lastMoveDirection;
}


void EntityLogicComponent::Tick(const Server* server, IEntity& entity) {
    const auto dt = server->GetDeltaTime_unprotected();
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

    auto updateLock = [&] {
        const auto spriteRenderingContext{entity.GetRenderingComponent()->GetSprite()->GetSpriteRenderingContext()};
        if (const auto frameInfo{spriteRenderingContext->GetCurrentFrame()}; frameInfo == spriteRenderingContext->GetCurrentFrameCount() -1)
            lock = false;
    };

    updateLock();

    for (auto &e : queueUpEvents) {
        events.emplace_back(std::move(e));
    }
    queueUpEvents.clear();
    if (entity.GetType() == EntityType::PLAYER) {
        const auto &player = dynamic_cast<Player&>(entity);
        if (player.IsGhostMode()) {
            interruptedEvents.clear();
            interruptedEvents.insert(EntityEvent::Type::ATTACK);
            interruptedEvents.insert(EntityEvent::Type::CLICK_ATTACK);
            interruptedEvents.insert(EntityEvent::Type::DAMAGE);
            interruptedEvents.insert(EntityEvent::Type::HEAL);
            interruptedEvents.insert(EntityEvent::Type::PLACE);
            interruptedEvents.insert(EntityEvent::Type::INVENTORY);
        }
        if (player.IsBeingRevived()) {
            interruptedEvents.clear();
            interruptedEvents.insert(EntityEvent::Type::MOVE);
            interruptedEvents.insert(EntityEvent::Type::MOVE_TO);
            interruptedEvents.insert(EntityEvent::Type::CLICK_MOVE);
        }
    }
    for (auto eventIndex{0}; eventIndex < events.size(); ++eventIndex) {
        if (!interrupted && !isInInterrupts(eventIndex)) process(eventIndex);
    }
    events.clear();
    interruptedEvents.clear();
    interrupted = false;
}

void EntityLogicComponent::AddEvent(std::unique_ptr<EntityEvent> eventData, bool priority) {
    if (priority) {
        events.insert(events.begin(), std::move(eventData));
        return;
    }
    events.push_back(std::move(eventData));
}

void EntityLogicComponent::QueueUpEvent(std::unique_ptr<EntityEvent> eventData, bool priority) {
    if (priority) {
        queueUpEvents.insert(queueUpEvents.begin(), std::move(eventData));;
        return;
    }
    queueUpEvents.push_back(std::move(eventData));
}

bool EntityLogicComponent::isInInterrupts(int eventIndex) const {
    return std::ranges::any_of(interruptedEvents,
                               [&] (const EntityEvent::Type interruptedEvent) {
                                   return interruptedEvent== events.at(eventIndex)->GetType();
                               });
}

bool EntityLogicComponent::Move(const float deltaTime, const float dX,const float dY, EntityCollisionComponent &collisionComponent, const Server* server) {
    if (lock) return false;
    lastCoordinates = coordinates;
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

void EntityLogicComponent::MoveTo(const float deltaTime,Coordinates targetCoordinates, IEntity* entity) {
    auto DestinationReached = [&] {
        entity->GetServer()->GetAiManager_unprotected().sendEvent(entity, AiEvent::ReachedDestination);
    };

    if (lock) {
        queueUpEvents.emplace_back(Event_MoveTo::Create(targetCoordinates.x, targetCoordinates.y));
        return;
    };

    const auto adjustment{entity->GetRenderingComponent()->CalculateCenterOffset(*entity)};
    auto CalculateDiff = [&](const Coordinates target) {
        return target - coordinates - adjustment;
    };

    Coordinates diffs = CalculateDiff(targetCoordinates);

    const float distance{std::sqrt(diffs.x * diffs.x + diffs.y * diffs.y)};
    const auto target {toTileCoordinates(targetCoordinates)};
    if (entity->GetServer()->GetMapValue_unprotected(
        static_cast<int>(target.x),
        static_cast<int>(target.y),
        WorldData::COLLISION_MAP) != 0) {

        // Adjust coordinates
        targetCoordinates.x = diffs.x >= 0 ? (target.x + 0.5f) * 32.0f : (target.x - 0.5f) * 32.0f;
        targetCoordinates.y = diffs.y >= 0 ? (target.y + 0.5f) * 32.0f : (target.y - 0.5f) * 32.0f;
        diffs = CalculateDiff(targetCoordinates);
    }

    if (distance <= threshold) {
        lastMoveDirection = {0.0f, 0.0f};
        DestinationReached();
        return;
    }

    diffs /= distance;

    auto CheckAt = [&](Coordinates diffsValue) -> bool {
        return EntityCollisionComponent::CheckCollisionAt(coordinates.x + diffsValue.x * speed * deltaTime, coordinates.y + diffsValue.y * speed * deltaTime, *entity);
    };

    if (CheckAt(diffs)) {
        if (lastMoveDirection != Coordinates{0.0f, 0.0f}
            && !CheckAt(lastMoveDirection)) {
            diffs = lastMoveDirection;
            }
        else if (!CheckAt({diffs.x, 0.0f})) {
            diffs.y = 0.0f;
            diffs.x = (diffs.x  > 0.0f) ? 1.0f : -1.0f;
        }
        else if (!CheckAt({0.0f, diffs.y})) {
            diffs.x = 0.0f;
            diffs.y = (diffs.y > 0.0f) ? 1.0f : -1.0f;
        }
        else {
            lastMoveDirection = {0.0f, 0.0f};
            DestinationReached();
            return;
        }
    }

    if (Move(deltaTime, diffs.x, diffs.y, *entity->GetCollisionComponent(), entity->GetServer())) {
        lastMoveDirection = diffs;
    }
    queueUpEvents.emplace_back(Event_MoveTo::Create(targetCoordinates.x, targetCoordinates.y));
}



void EntityLogicComponent::PerformAttack(IEntity* entity, const int attackType, const int damage) const {
    if (lock) return;
    if (attackType < 0) return;
    const auto renderingComponent = entity->GetRenderingComponent();
    const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(angle);
    renderingComponent->PlayAnimation(AnimationType::ATTACK, direction, attackType,true);
    entity->GetLogicComponent()->SetLock();
    const auto reach{entity->GetReach()};
    const auto entityCenter{entity->GetEntityCenter()};

    std::vector<Coordinates> attackPoints;
    int circleStart{0};
    int circleEnd{360};

    if (direction == Direction::UP) {
        circleStart = 180;
        circleEnd = 360;
    }
    else if (direction == Direction::DOWN) {
        circleStart = 0;
        circleEnd = 180;
    }
    else if (direction == Direction::LEFT) {
        circleStart = 90;
        circleEnd = 270;
    }
    else if (direction == Direction::RIGHT) {
        circleStart = 270;
        circleEnd = 450;
    }

    attackPoints.reserve((circleEnd - circleStart) * reach);
    for (int i{circleStart}; i < circleEnd; ++i) {
        const auto rad = static_cast<float>(i * M_PI / 180.0f);
        for (int j{3}; j < reach; ++j)
            attackPoints.push_back({
                entityCenter.x + std::cos(rad) * static_cast<float>(j),
                entityCenter.y + std::sin(rad) * static_cast<float>(j)

            });
    }

    entity->GetServer()->ApplyDamageAt_unprotected(damage, attackPoints, entity->GetId());
}


//EntityHealthComponent methods
void EntityHealthComponent::Heal(const int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void EntityHealthComponent::Tick(const float deltaTime) {
    timeSinceLastDamage += deltaTime;
    if (IsDead() || health >= maxHealth) return;
    if (timeSinceLastDamage < 5*timeToRegenerate) {
        return;
    }
    regenerationTime += deltaTime;
    if (regenerationTime >= timeToRegenerate && health < maxHealth) {
        regenerationTime = 0.0f;
        health += 5;
        if (health > maxHealth) health = maxHealth;
    }
}

void EntityHealthComponent::TakeDamage(const int damage, IEntity& entity) {
    if (IsDead() || damage <= 0) return;
    health -= damage;

    auto renderingComponent{entity.GetRenderingComponent()};
    const auto direction{EntityRenderingComponent::GetDirectionBaseOnAngle(entity.GetLogicComponent()->GetAngle())};
    const auto logicComponent{entity.GetLogicComponent()};

    renderingComponent->PlayAnimation(AnimationType::HURT, direction, 1, true);
    logicComponent->SetLock(); //Lock entity during hurt animation

    if (health < 0) health = 0;
    if (IsDead()) {
        entity.GetLogicComponent()->SetInterrupted(true);
        if (entity.GetType() == EntityType::PLAYER) {
            dynamic_cast<Player*>(&entity)->SetGhostMode(true);
            entity.GetRenderingComponent()->PlayReversedAnimation(AnimationType::DEATH,Direction::OMNI, 1, true);
        }
        else renderingComponent->PlayAnimation(AnimationType::DEATH,direction, 1, true);
        logicComponent->QueueUpEvent(Event_Death::Create());
        return;
    }

    if (auto* server = entity.GetServer()) {
        server->GetAiManager_unprotected().sendEvent(&entity, AiEvent::TookDamage);
        if (health <= maxHealth / 5 && health > 0) {
            server->GetAiManager_unprotected().sendEvent(&entity, AiEvent::LowHealth);
        }
    }
    timeSinceLastDamage = 0.0f;
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
bool EntityHealthComponent::IsDead() const {
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
        auto targetDX{data->dX};
        auto targetDY{data->dY};
        const auto distance = std::sqrt(data->dX * data->dX + data->dY * data->dY);
        if (distance > 0.0f) {
            targetDX /= distance;
            targetDY /= distance;
        } // Normalizace vektoru
        if (!entity->GetLogicComponent()->Move(data->GetDeltaTime(), targetDX, targetDY, *entity->GetCollisionComponent(), entity->GetServer()))
            entity->GetServer()->GetAiManager_unprotected().sendEvent(entity, AiEvent::MovementStuck);

    });
    instance->bindings.insert_or_assign(EntityEvent::Type::MOVE_TO, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_MoveTo*>(e);
        entity->GetLogicComponent()->MoveTo(data->GetDeltaTime(), {data->targetX, data->targetY}, entity);

    });
    instance->bindings.insert_or_assign(EntityEvent::Type::CLICK_MOVE, [](IEntity* entity, const EntityEvent *e) {
       const auto data = dynamic_cast<const Event_ClickMove*>(e);
        entity->GetLogicComponent()->interrupted = true;
        entity->GetLogicComponent()->MoveTo(data->GetDeltaTime(), {data->targetX, data->targetY}, entity);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::HEAL, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_Heal*>(e);
        entity->GetHealthComponent()->Heal(data->amount);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::DAMAGE, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_Damage*>(e);
        entity->GetHealthComponent()->TakeDamage(data->amount, *entity);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::ATTACK, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_Attack*>(e);
        entity->GetLogicComponent()->PerformAttack(entity, data->attackType, data->damage);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::CLICK_ATTACK, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_ClickAttack*>(e);
        entity->GetLogicComponent()->PerformAttack(entity, data->attackType, data->damage);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::SET_ANGLE, [](IEntity* entity, const EntityEvent *e) {
        const auto data =  dynamic_cast<const Event_SetAngle*>(e);
        entity->GetLogicComponent()->SetAngle(data->angle);
    });
    instance->bindings.insert_or_assign(EntityEvent::Type::DEATH, [](IEntity* entity, const EntityEvent *e) {
        //TODO: Drop loot
        const auto logicComponent{entity->GetLogicComponent()};
        logicComponent->SetInterrupted(true);
        if (logicComponent->lock) {
            logicComponent->QueueUpEvent(Event_Death::Create());
            return;
        }
        if (entity->GetType()==EntityType::PLAYER) return;
        entity->DropItemsOnDeath();
        entity->GetServer()->RemoveEntity_unprotected(entity->GetId());
    });

}

// EntityInventoryComponent methods
int EntityInventoryComponent::nextInventoryId = 0;

EntityInventoryComponent::EntityInventoryComponent() {
    inventoryId = nextInventoryId++;
}

bool EntityInventoryComponent::addItem(std::unique_ptr<Item> item) {
    if (!item) return false;

    // Try to stack with existing item if stackable
    if (item->isStackable()) {
        int existingSlot = findStackableSlot(item.get());
        if (existingSlot != -1) {
            Item* existingItem = items[existingSlot].get();
            int spaceLeft = existingItem->getMaxStackSize() - existingItem->getStackSize();

            if (spaceLeft > 0) {
                int toAdd = std::min(spaceLeft, item->getStackSize());
                existingItem->addToStack(toAdd);

                if (toAdd >= item->getStackSize()) {
                    return true;
                }
                item->removeFromStack(toAdd);
            }
        }
    }

    int emptySlot = findEmptySlot();
    if (emptySlot == -1) {
        return false;  // Inventory full
    }

    items[emptySlot] = std::move(item);
    return true;
}

bool EntityInventoryComponent::removeItem(int slotIndex, int count) {
    auto it = items.find(slotIndex);
    if (it == items.end()) {
        return false;
    }

    Item* item = it->second.get();

    if (item->isStackable() && item->getStackSize() > count) {
        item->removeFromStack(count);
    } else {
        items.erase(it);
    }

    return true;
}

bool EntityInventoryComponent::moveItem(int fromSlot, int toSlot) {
    if (fromSlot == toSlot) return false;

    auto fromIt = items.find(fromSlot);
    if (fromIt == items.end()) return false;

    auto toIt = items.find(toSlot);

    if (toIt == items.end()) {
        // Target slot is empty - move item
        items[toSlot] = std::move(fromIt->second);
        items.erase(fromIt);
    } else {
        // Target slot has item - try to stack or swap
        Item* fromItem = fromIt->second.get();
        Item* toItem = toIt->second.get();

        if (fromItem->isStackable() && toItem->isStackable() &&
            fromItem->getName() == toItem->getName()) {

            int fromAmount = fromItem->getStackSize();
            int maxStack = toItem->getMaxStackSize();
            int spaceInTarget = maxStack - toItem->getStackSize();

            if (spaceInTarget > 0) {
                int transferAmount = std::min(fromAmount, spaceInTarget);
                toItem->addToStack(transferAmount);
                fromItem->removeFromStack(transferAmount);

                if (fromItem->getStackSize() <= 0) {
                    items.erase(fromIt);
                }
            } else {
                std::swap(fromIt->second, toIt->second);
            }
        } else {
            std::swap(fromIt->second, toIt->second);
        }
    }

    return true;
}

void EntityInventoryComponent::clearSlot(int slotIndex) {
    items.erase(slotIndex);
}

Item* EntityInventoryComponent::getItem(int slotIndex) const {
    auto it = items.find(slotIndex);
    if (it != items.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool EntityInventoryComponent::hasItem(int slotIndex) const {
    return items.find(slotIndex) != items.end();
}

int EntityInventoryComponent::findEmptySlot() const {
    for (int i = 0; i < totalSlots; i++) {
        if (items.find(i) == items.end()) {
            return i;
        }
    }
    return -1;
}

int EntityInventoryComponent::findStackableSlot(Item* item) const {
    if (!item || !item->isStackable()) return -1;

    for (const auto& [slotIndex, slotItem] : items) {
        if (slotItem->getName() == item->getName() &&
            slotItem->isStackable() &&
            slotItem->getStackSize() < slotItem->getMaxStackSize()) {
            return slotIndex;
        }
    }
    return -1;
}

int EntityInventoryComponent::countItems(ItemType type) const {
    int count = 0;
    for (const auto& [slotIndex, item] : items) {
        if (item->getType() == type) {
            count += item->getStackSize();
        }
    }
    return count;
}

int EntityInventoryComponent::countMaterials(MaterialType materialType) const {
    int count = 0;
    for (const auto& [slotIndex, item] : items) {
        if (item->getType() == ItemType::MATERIAL) {
            auto* material = dynamic_cast<Material*>(item.get());
            if (material && material->getMaterialType() == materialType) {
                count += item->getStackSize();
            }
        }
    }
    return count;
}

