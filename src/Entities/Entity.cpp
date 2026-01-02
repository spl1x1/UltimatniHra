//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "../../include/Entities/Entity.h"
#include "../../include/Entities/EntityScripts.h"
#include <cmath>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"
#include <queue>
#include <unordered_map>
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

void EntityLogicComponent::BindScript(const std::string &scriptName, const ScriptData &scriptData) {
    _scriptBindings[scriptName] = scriptData;
}

void EntityLogicComponent::UnbindScript(const std::string &scriptName) {
    _scriptBindings.erase(scriptName);
}

EntityLogicComponent::ScriptData EntityLogicComponent::GetBoundScript(const std::string &scriptName) const {
    if (_scriptBindings.contains(scriptName)) {
        return _scriptBindings.at(scriptName);
    }
    return ScriptData{};
}

void EntityLogicComponent::SetTasks(const std::vector<TaskData> &newTasks) {
    _tasks = newTasks;
}

void EntityLogicComponent::SetTask(const TaskData &newTask) {
    _tasks.push_back(newTask);
}

std::vector<TaskData> EntityLogicComponent::GetTasks() const {
    return  _tasks;
}

void EntityLogicComponent::SetEvents(const std::vector<EventData> &newEvents) {
    _events = newEvents;
}

std::vector<EventData> EntityLogicComponent::GetEvents() const {
    return _events;
}


void EntityLogicComponent::SetAngle(const int newAngle) {
    _angle = newAngle;
}

int EntityLogicComponent::GetAngle() const {
    return _angle;
}

void EntityLogicComponent::SetSpeed(float newSpeed) {
    if (newSpeed < 0.0f) return;
    _speed = newSpeed;
}

float EntityLogicComponent::GetSpeed() const {
    return _speed;
}


void EntityLogicComponent::SetAngleBasedOnMovement(const float dX, const float dY) {
    _angle = static_cast<int>(std::floor(std::atan2(dX, dY) * 180.0f / M_PI));
    if (_angle < 0) _angle += 360;
}

void EntityLogicComponent::SetCoordinates(const Coordinates &newCoordinates) {
    _coordinates = newCoordinates;
}

Coordinates EntityLogicComponent::GetCoordinates() const {
    return _coordinates;
}


void EntityLogicComponent::Tick(const Server* server, IEntity& entity) {
    for (auto taskIndex{0}; taskIndex < _tasks.size(); ++taskIndex) {
        HandleTask(server, entity, taskIndex);
    }
    for (auto eventIndex{0}; eventIndex < _events.size(); ++eventIndex) {
        HandleEvent(server, entity, eventIndex);
    }
}

void EntityLogicComponent::AddEvent(const EventData &eventData) {
    _events.push_back(eventData);
}

void EntityLogicComponent::RegisterScriptBinding(const std::string &scriptName, const ScriptData &scriptData) {
    _scriptBindings[scriptName] = scriptData;
}

EntityLogicComponent::EntityLogicComponent(const Coordinates &coordinates) : _coordinates(coordinates) {

    //Register MOVE_TO script binding
     RegisterScriptBinding("MOVE_TO", {
        .scriptName = "MOVE_TO",
        .function = EntityScripts::MoveToScript
    });
}


bool EntityLogicComponent::Move(const float deltaTime, const float dX,const float dY, EntityCollisionComponent &collisionComponent, const Server* server) {
    const float newX{_coordinates.x + dX * _speed* deltaTime};
    const float newY{_coordinates.y + dY * _speed * deltaTime};

    collisionComponent.CheckCollision(newX, newY, server);
    const EntityCollisionComponent::HitboxData hitbox{*collisionComponent.GetHitbox()};

    if (hitbox.colliding && !hitbox.disabledCollision) return false;
    this->_coordinates.x = newX;
    this->_coordinates.y = newY;
    SetAngleBasedOnMovement(dX, dY);
    return true;
}

void EntityLogicComponent::HandleEvent(const Server* server, IEntity &entity, int eventIndex) {
    auto data = _events.at(eventIndex);
    switch (data.type) {
        case Event::MOVE:
            Move(data.dt, data.data.move.dX, data.data.move.dY, *entity.GetCollisionComponent(), server);
            break;
        case Event::CHANGE_COLLISION:
            entity.GetCollisionComponent()->SwitchCollision();
            break;
        case Event::DAMAGE:
            entity.GetHealthComponent()->TakeDamage(data.data.healthChange.amount);
            break;
        default:
            break;
    }
    _events.erase(_events.begin());
}

void EntityLogicComponent::HandleTask(const Server* server, IEntity& entity, int taskIndex) {
    TaskData data = _tasks.at(taskIndex);
    if (data.status == TaskData::Status::PENDING) {
        ProcessNewTask(server, entity);
        return;
    }
    if (data.status == TaskData::Status::DONE || data.status == TaskData::Status::FAILED) {
        //Implement logger call
        _tasks.erase(_tasks.begin());
        return;
    }
    if (data.status == TaskData::Status::IN_PROGRESS) {
        if (!_scriptBindings.contains(data.taskName)) return;
        _scriptBindings[data.taskName].function(entity, data);
    }
}

void EntityLogicComponent::ProcessNewTask(const Server* server, IEntity& entity) {
    TaskData currentTask = _tasks.front();

    if (!_scriptBindings.contains(currentTask.taskName)) return;
    _scriptBindings[currentTask.taskName].function(entity, currentTask);
    currentTask.status = TaskData::Status::IN_PROGRESS;
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

