//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Player.hpp"
#include "../../include/Sprites/PlayerSprite.hpp"
#include <memory>


//PlayerNew

void Player::Tick() {
    const auto oldCoords{_entityLogicComponent.GetCoordinates()};
    const auto deltaTime = _server->getDeltaTime_unprotected();
    _entityRenderingComponent.SetDirectionBaseOnAngle(_entityLogicComponent.GetAngle());
    _entityRenderingComponent.Tick(deltaTime);
    _entityLogicComponent.Tick(deltaTime, _server, _entityCollisionComponent, this);
    if (oldCoords.x == _entityLogicComponent.GetCoordinates().x && oldCoords.y == _entityLogicComponent.GetCoordinates().y)
        _entityRenderingComponent.SetAnimation(AnimationType::IDLE);
    else
        _entityRenderingComponent.SetAnimation(AnimationType::RUNNING);
}

void Player::Render(SDL_Renderer &windowRenderer, SDL_FRect &cameraRectangle,
    std::unordered_map<std::string, SDL_Texture *> &textures) {
    _entityRenderingComponent.Render(&windowRenderer,_entityLogicComponent.GetCoordinates(),cameraRectangle,textures);
}

void Player::Create(const std::shared_ptr<Server>& server) {
    auto player = std::make_shared<Player>(server, server->getSpawnPoint());
    server->addPlayer(player);
}

void Player::Load(const std::shared_ptr<Server>& server) {
    // TODO: Load player data from save
}

void Player::Move(float dX, float dY) {
    const auto oldCoords{_entityLogicComponent.GetCoordinates()};
    _entityLogicComponent.Move(_server->getDeltaTime(),dX,dY, _entityCollisionComponent, _server);
    _entityRenderingComponent.SetDirectionBaseOnAngle(_entityLogicComponent.GetAngle());
    if (oldCoords.x == _entityLogicComponent.GetCoordinates().x && oldCoords.y == _entityLogicComponent.GetCoordinates().y)
        _entityRenderingComponent.SetAnimation(AnimationType::IDLE);
    else
        _entityRenderingComponent.SetAnimation(AnimationType::RUNNING);
}

void Player::HandleTask(TaskData data) {
    _entityLogicComponent.SetTask(data);
}

void Player::SetCoordinates(const Coordinates &newCoordinates) {
    _entityLogicComponent.SetCoordinates(newCoordinates);
}

void Player::SetAngle(const int newAngle) {
    _entityLogicComponent.SetAngle(newAngle);
    _entityRenderingComponent.SetDirectionBaseOnAngle(newAngle);
}

void Player::SetSpeed(float newSpeed) {
    _entityLogicComponent.SetSpeed(newSpeed);
}

void Player::SetTask(const int index) {
    _entityLogicComponent.SwitchTask(index);
}

void Player::RemoveTask(const int index) {
    _entityLogicComponent.RemoveTask(index);
}

void Player::SetEntityCollision(bool disable) {
    _entityCollisionComponent.DisableCollision(disable);
}

void Player::AddEvent(const EventData &eventData) {
    _entityLogicComponent.AddEvent(eventData);
}

Coordinates Player::GetCoordinates() const {
    return _entityLogicComponent.GetCoordinates();
}

CollisionStatus Player::GetCollisionStatus() const {
    return _entityCollisionComponent.GetCollisionStatus();
}

int Player::GetAngle() const {
    return _entityLogicComponent.GetAngle();
}

TaskData Player::GetTask() const {
    return _entityLogicComponent.GetTask(0);
}

std::vector<TaskData> Player::GetTasks() const {
    return _entityLogicComponent.GetTasks();
}

std::vector<EventData> Player::GetEvents() const {
    return _entityLogicComponent.GetEvents();
}

EntityCollisionComponent * Player::GetCollisionComponent() {
    return &_entityCollisionComponent;
}

EntityLogicComponent * Player::GetLogicComponent() {
    return &_entityLogicComponent;
}

EntityHealthComponent * Player::GetHealthComponent() {
    return &_entityHealthComponent;
}

EntityRenderingComponent * Player::GetRenderingComponent() {
    return &_entityRenderingComponent;
}

EntityInventoryComponent * Player::GetInventoryComponent() {
    return &_entityInventoryComponent;
}

std::shared_ptr<Server> Player::GetServer() const {
    return _server;
}

Player::Player(std::shared_ptr<Server> server, const Coordinates& coordinates):
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
    _entityLogicComponent.SetSpeed(200.0f);
    _entityRenderingComponent.SetAnimation(AnimationType::IDLE);
    _entityCollisionComponent.SetHitbox(playerHitboxData);
}
