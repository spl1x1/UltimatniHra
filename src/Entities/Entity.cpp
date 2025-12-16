//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#include "../../include/Entities/Entity.h"
#include <cmath>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"
#include <queue>
#include <unordered_map>
#include <algorithm>

//EntityRenderingComponent methods
void EntityRenderingComponent::Render(const SDL_Renderer* renderer, const Coordinates &entityCoordinates, const SDL_FRect &cameraRectangle, std::unordered_map<std::string, SDL_Texture*> texturePool) const {
    if (!_sprite) return;

    const auto renderingContex = _sprite->getFrame();

    _rect->x = entityCoordinates.x - cameraRectangle.x;
    _rect->y = entityCoordinates.y - cameraRectangle.y;
    _rect->w = static_cast<float>(_sprite->getWidth());
    _rect->h = static_cast<float>(_sprite->getHeight());

    SDL_RenderTexture(const_cast<SDL_Renderer*>(renderer), texturePool[std::get<0>(renderingContex)], std::get<1>(renderingContex), _rect.get());
}
void EntityRenderingComponent::Tick(const float deltaTime) const {
    if (!_sprite) return;
    _sprite->Tick(deltaTime);
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
            .collisionDisabled = _hitbox.disableCollision
    };
}

void EntityCollisionComponent::SetHitbox(const HitboxData &hitbox){
    _hitbox = hitbox;
}

void EntityCollisionComponent::DisableCollision(const bool Switch){
    _hitbox.disableCollision = Switch;
}


bool EntityCollisionComponent::CheckCollision(const float newX, const float newY, const std::shared_ptr<Server>& server) {
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

bool EntityCollisionComponent::CheckCollisionAt(const float newX, const float newY, const std::shared_ptr<Server>& server) const{
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
    if (_scriptBindings.find(scriptName) != _scriptBindings.end()) {
        return _scriptBindings.at(scriptName);
    }
    return ScriptData{};
}


void EntityLogicComponent::SetPathPoints(const std::vector<Coordinates> &newPathPoints) {
    _pathPoints = newPathPoints;
}

std::vector<Coordinates> EntityLogicComponent::GetPathPoints() const {
    return _pathPoints;
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

TaskData EntityLogicComponent::GetTask(int index) const {
    if (index < 0 || index >= _tasks.size()) return {};
    return _tasks[index];
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

void EntityLogicComponent::SwitchTask(int index) {
    if (index < 0 || index >= static_cast<int>(_tasks.size())) return;
    _tasks.front().status = TaskData::Status::PENDING;
    std::ranges::rotate(_tasks, _tasks.begin() + index);
}

void EntityLogicComponent::RemoveTask(int index) {
    if (index < 0 || index >= static_cast<int>(_tasks.size())) return;
    _tasks.erase(_tasks.begin() + index);
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

void EntityLogicComponent::MoveTo(const float targetX, const float targetY) {
    const float deltaX{targetX - oldDX};
    const float deltaY{targetY - oldDY};
    oldDX= 0.0f;
    oldDY = 0.0f;

    if (const float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY); distance > threshold) {
        oldDX = deltaX / distance;
        oldDY = deltaY / distance;
    }

    AddEvent(EventData{
            .type = Event::MOVE,
            .data = {oldDX, oldDY}
    });
}

void EntityLogicComponent::MakePath(float targetX, float targetY, const std::shared_ptr<Server> &server,const EntityCollisionComponent &collisionComponent) {
    _pathPoints.clear();

    const int startX = static_cast<int>(std::floor(_coordinates.x / 32.0f));
    const int startY = static_cast<int>(std::floor(_coordinates.y / 32.0f));
    const int endX = static_cast<int>(std::floor (targetX / 32.0f));
    const int endY = static_cast<int>(std::floor (targetY / 32.0f));

    if (endX <0 || endY <0 || endX >= MAPSIZE || endY >= MAPSIZE) return;
    if (server->getMapValue(endX, endY, WorldData::COLLISION_MAP) != 0) return;

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> pq;
    std::unordered_map<GridPoint, bool, GridPointHash> visited;
    std::unordered_map<GridPoint, GridPoint, GridPointHash> parent;
    std::unordered_map<GridPoint, float, GridPointHash> cost;

    GridPoint start = {startX, startY};
    GridPoint end = {endX, endY};

    pq.push({startX, startY, 0.0f});
    cost[start] = 0.0f;

    bool pathFound = false;

    while (!pq.empty()) {
        PathNode current = pq.top();
        pq.pop();

        GridPoint currentPoint = {current.x, current.y};

        if (visited[currentPoint]) continue;
        visited[currentPoint] =true;

        if (current.x == endX && current.y == endY) {
            pathFound = true;
            break;
        }
        for (int i = 0; i < 8;i++) {
            constexpr int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
            constexpr int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
            constexpr float moveCost[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.414f, 1.414f, 1.414f, 1.414f};
            const int nx = current.x + dx[i];
            const int ny = current.y + dy[i];
            GridPoint neeighbour = {nx, ny};

            if (nx < 0 || ny < 0 || nx >= MAPSIZE || ny >= MAPSIZE) continue;

            if (collisionComponent.CheckCollisionAt(static_cast<float>(nx),static_cast<float>(ny),server)) continue;
            if (visited[neeighbour]) continue;

            if (i >= 4) {
                const int adjX1 = current.x + dx[i];
                const int adjY1 = current.y;
                const int adjX2 = current.x;
                const int adjY2 = current.y + dy[i];

                if (collisionComponent.CheckCollisionAt(static_cast<float>(adjX1),static_cast<float>(adjY1),server) ||
                    collisionComponent.CheckCollisionAt(static_cast<float>(adjX2),static_cast<float>(adjY2),server)) {
                    continue;
                    }
            }
            const float newCost = current.cost + moveCost[i];

            if (!cost.contains(neeighbour) || newCost < cost[neeighbour]) {
                cost[neeighbour] = newCost;
                parent[neeighbour] = currentPoint;
                pq.push({nx, ny, newCost});
            }
        }
    }
    if (pathFound) {
        std::vector<GridPoint> fullPath;
        GridPoint current = end;

        while (current != start) {
            fullPath.push_back(current);
            current = parent[current];
        }
        fullPath.push_back(start);

        std::ranges::reverse(fullPath);

        if (fullPath.size() > 1) {
            _pathPoints.push_back({
                static_cast<float>(fullPath[0].x * 32 + 16),
                static_cast<float>(fullPath[0].y * 32 + 16)
            });

            for (size_t i = 1; i < fullPath.size() - 1; i++) {
                const int prevDx = fullPath[i].x - fullPath[i-1].x;
                const int prevDy = fullPath[i].y - fullPath[i-1].y;
                const int nextDx = fullPath[i+1].x - fullPath[i].x;
                const int nextDy = fullPath[i+1].y - fullPath[i].y;

                if (prevDx != nextDx || prevDy != nextDy) {
                    _pathPoints.push_back({
                        static_cast<float>(fullPath[i].x * 32 + 16),
                        static_cast<float>(fullPath[i].y * 32 + 16)
                    });
                }
            }

            _pathPoints.push_back({
                static_cast<float>(fullPath.back().x * 32 + 16),
                static_cast<float>(fullPath.back().y * 32 + 16)
            });
        }
    }


}

void EntityLogicComponent::PathMovement(EntityCollisionComponent &collisionComponent, const std::shared_ptr<Server> &server) {
    if (_pathPoints.empty()) return;
    const Coordinates targetPoint = _pathPoints.front();

    MoveTo(targetPoint.x, targetPoint.y);

    if (std::abs(_coordinates.x - targetPoint.x) <= threshold && std::abs(_coordinates.y - targetPoint.y) <= threshold) {
        _pathPoints.erase(_pathPoints.begin());
    }
}

void EntityLogicComponent::Tick(const float deltaTime, const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent, IEntity* entity) {
    if (!_events.empty()) HandleEvent(_events.front(), server, collisionComponent);
    if (!_tasks.empty()) HandleTask(_tasks.front(), server, collisionComponent, entity);
}

void EntityLogicComponent::AddEvent(const EventData &eventData) {
    _events.push_back(eventData);
}

void EntityLogicComponent::RegisterScriptBinding(const std::string &scriptName, const ScriptData &scriptData) {
    _scriptBindings[scriptName] = scriptData;
}

EntityLogicComponent::EntityLogicComponent(const Coordinates &coordinates) : _coordinates(coordinates) {

    //Register MOVE_TO script binding
    auto moveScript = ScriptData{
            .script = Script::CPP,
            .functionCPP = []( IEntity *entity){
                auto logicComponent = entity->GetLogicComponent();
                auto collisionComponent = entity->GetCollisionComponent();
                if (!logicComponent) return;
                logicComponent->PathMovement(*collisionComponent, entity->GetServer());

            }
    };
    RegisterScriptBinding("MOVE_TO", moveScript);

}


bool EntityLogicComponent::Move(const float deltaTime, const float dX,const float dY, EntityCollisionComponent &collisionComponent, const std::shared_ptr<Server> &server) {
    const float newX{_coordinates.x + dX * _speed* deltaTime};
    const float newY{_coordinates.y + dY * _speed * deltaTime};

    collisionComponent.CheckCollision(newX, newY, server);
    const EntityCollisionComponent::HitboxData hitbox{*collisionComponent.GetHitbox()};

    if (hitbox.colliding && !hitbox.disableCollision) return false;
    this->_coordinates.x = newX;
    this->_coordinates.y = newY;
    SetAngleBasedOnMovement(dX, dY);
    return true;
}

void EntityLogicComponent::HandleEvent(const EventData &data, const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent) {
    switch (data.type) {
        case Event::MOVE:
            Move(data.dt, data.data.move.dX, data.data.move.dY, collisionComponent, server);

            break;
        default:
            break;
    }
    _events.erase(_events.begin());
}

void EntityLogicComponent::HandleTask(const TaskData &data, const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent, IEntity* entity) {
    if (data.status == TaskData::Status::PENDING) {
        ProcessNewTask(server, collisionComponent, entity);
        return;
    }
    if (data.status == TaskData::Status::DONE || data.status == TaskData::Status::FAILED) {
        //Implement logger call
        _tasks.erase(_tasks.begin());
        return;
    }
    if (data.status == TaskData::Status::IN_PROGRESS) {
        if (_scriptBindings.find(data.taskName) == _scriptBindings.end()) return;
        ScriptData &scriptData = _scriptBindings[data.taskName];

        if (scriptData.script == Script::NOT_IMPLEMENTED) return;

        if (scriptData.script == Script::CPP) {
            scriptData.functionCPP(entity);
            return;
        }
        // TODO: Implement Lua script execution

        return;
    }

}

void EntityLogicComponent::ProcessNewTask(const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent, IEntity* entity) {
    TaskData &currentTask = _tasks.front();

    if (_scriptBindings.find(currentTask.taskName) == _scriptBindings.end()) return;

    ScriptData &scriptData = _scriptBindings[currentTask.taskName];
    if (scriptData.script == Script::NOT_IMPLEMENTED) {
        return;
    }
    if (scriptData.script == Script::CPP) {
        currentTask.status = TaskData::Status::IN_PROGRESS;
        scriptData.functionCPP(entity);
        currentTask.Iterations++;
        return;
    }
        // TODO: Implement Lua script execution

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

