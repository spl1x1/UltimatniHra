//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Player.hpp"
#include "../../include/Sprites/PlayerSprite.hpp"
#include <memory>
#include <utility>


//PlayerNew

void PlayerNew::Tick() {
    const auto deltaTime = _server->getDeltaTime();
    _entityRenderingComponent.Tick(deltaTime);
    _entityLogicComponent.Tick(deltaTime, _server, _entityCollisionComponent);
}

void PlayerNew::Render(SDL_Renderer &windowRenderer, SDL_FRect &cameraRectangle,
    std::unordered_map<std::string, SDL_Texture *> &textures) {
    _entityRenderingComponent.Render(&windowRenderer,_entityLogicComponent.GetCoordinates(),cameraRectangle,textures);
}

void PlayerNew::Create() {
}

void PlayerNew::Load() {
}

void PlayerNew::Move(float dX, float dY) {
    const auto oldCoords{_entityLogicComponent.GetCoordinates()};
    _entityLogicComponent.Move(dX,dY, _entityCollisionComponent, _server);
    _entityRenderingComponent.SetDirectionBaseOnAngle(_entityLogicComponent.GetAngle());
    if (oldCoords.x == _entityLogicComponent.GetCoordinates().x && oldCoords.y == _entityLogicComponent.GetCoordinates().y)
        _entityRenderingComponent.SetAnimation(AnimationType::IDLE);
    else
        _entityRenderingComponent.SetAnimation(AnimationType::RUNNING);
}

void PlayerNew::HandleTask(TaskData data) {
    _entityLogicComponent.SetTask(data);
}

void PlayerNew::SetCoordinates(const Coordinates &newCoordinates) {
    _entityLogicComponent.SetCoordinates(newCoordinates);
}

void PlayerNew::SetAngle(const int newAngle) {
    _entityLogicComponent.SetAngle(newAngle);
    _entityRenderingComponent.SetDirectionBaseOnAngle(newAngle);
}

void PlayerNew::SetSpeed(float newSpeed) {
    _entityLogicComponent.SetSpeed(newSpeed);
}

void PlayerNew::SetTask(const int index) {
    _entityLogicComponent.SwitchTask(index);
}

void PlayerNew::RemoveTask(const int index) {
    _entityLogicComponent.RemoveTask(index);
}

Coordinates PlayerNew::GetCoordinates() const {
    return _entityLogicComponent.GetCoordinates();
}

CollisionStatus PlayerNew::GetCollisionStatus() const {
    return _entityCollisionComponent.GetCollisionStatus();
}

int PlayerNew::GetAngle() const {
    return _entityLogicComponent.GetAngle();
}

TaskData PlayerNew::GetTask() const {
    return _entityLogicComponent.GetTask(0);
}

std::vector<TaskData> PlayerNew::GetTasks() const {
    return _entityLogicComponent.GetTasks();
}

std::vector<EventData> PlayerNew::GetEvents() const {
    return _entityLogicComponent.GetEvents();
}

PlayerNew::PlayerNew(std::shared_ptr<Server> server, const Coordinates& coordinates):
    _entityRenderingComponent(std::make_unique<PlayerSprite>()),
    _entityLogicComponent(coordinates),
    _entityCollisionComponent(EntityCollisionComponent::HitboxData{}),
    _entityHealthComponent(100.0f, 100.0f) {
    _server = server; // Should be copied

    EntityCollisionComponent::HitboxData playerHitboxData = {
        {
            {32, 32}, // TOP_LEFT
            {64, 32}, // TOP_RIGHT
            {64, 65}, // BOTTOM_RIGHT
            {32, 65} // BOTTOM_LEFT
        },
        false, // disableCollision
        false // colliding
    };

    _entityCollisionComponent.SetHitbox(playerHitboxData);
}

//Player

void Player::handleEvent(PlayerEvent e) {
    switch (e.type) {
        case PlayerEvents::MOVE:
            Move(e.data1, e.data2, e.deltaTime);
            break;
        default:
            break;
    }
}

Player::Player(int id, float maxHealth, Coordinates coordinates ,const std::shared_ptr<Server>& server ,float speed): Entity(id ,maxHealth,coordinates, EntityType::PLAYER, server ,speed, std::make_unique<PlayerSprite>()) {
    Hitbox playerHitbox = {
        {
            {32, 32}, // TOP_LEFT
            {64 ,32}, // TOP_RIGHT
            {64, 65}, // BOTTOM_RIGHT
            {32,65} // BOTTOM_LEFT
        }
    };
    SetHitbox(playerHitbox);
    setSpriteOffsetX(47);
    setSpriteOffsetY(47);
};

void Player::ClientInit(const std::shared_ptr<Server>& server) {
    auto player = std::make_shared<Player>(0, 100.0f, server->getSpawnPoint(), server, 200.0f);
    server->addPlayer(player);
}
