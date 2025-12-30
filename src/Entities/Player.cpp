//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Player.hpp"
#include "../../include/Sprites/PlayerSprite.hpp"
#include "../../include/Application/SaveGame.h"
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

void Player::Create(Server* server, int slotId) {
    auto player = std::make_shared<Player>(server, server->getSpawnPoint());
    server->addPlayer(player);
    SaveManager::getInstance().setCurrentSlot(slotId);
}

void Player::Load(Server* server, int slotId) {
    // TODO: Load player data from save
    auto player = std::make_shared<Player>(server, server->getSpawnPoint());
    server->addPlayer(player);

    // Load saved data into player
    SaveManager::getInstance().loadGame(slotId, server);
}
void Player::Save(Server* server) {
    int currentSlot = SaveManager::getInstance().getCurrentSlot();
    if (currentSlot >= 0) {
        SaveManager::getInstance().saveGame(currentSlot, server);
    }
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

void Player::SetTask(int index) {
    // Not implemented for Player
}

void Player::RemoveTask(int index) {
    // Not implemented for Player
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

Server* Player::GetServer() const {
    return _server;
}

Player::Player(Server* server, const Coordinates& coordinates):
    _entityRenderingComponent(std::make_unique<PlayerSprite>()),
    _entityCollisionComponent(EntityCollisionComponent::HitboxData{}),
    _entityLogicComponent(coordinates),
    _entityHealthComponent(100.0f, 100.0f) {
    _server = server;

    EntityCollisionComponent::HitboxData playerHitboxData = {
        {
            {41, 34}, // TOP_LEFT
            {55, 34}, // TOP_RIGHT
            {55, 60}, // BOTTOM_RIGHT
            {41, 60} // BOTTOM_LEFT
        },
        false, // disableCollision
        false // colliding
    };
    _entityLogicComponent.SetSpeed(200.0f);
    _entityRenderingComponent.SetAnimation(AnimationType::IDLE);
    _entityCollisionComponent.SetHitbox(playerHitboxData);
}
