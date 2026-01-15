//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Player.hpp"
#include "../../include/Sprites/PlayerSprite.hpp"
#include "../../include/Application/SaveGame.h"
#include <memory>


//PlayerNew

void Player::Tick() {
    const auto deltaTime = _server->getDeltaTime_unprotected();
    const auto oldCoordinates = _entityLogicComponent.GetCoordinates();
    _entityRenderingComponent.Tick(deltaTime);
    _entityLogicComponent.Tick(_server, *this);
    const auto newCoordinates = _entityLogicComponent.GetCoordinates();
    if (_entityLogicComponent.IsInterrupted()) return;

    if (oldCoordinates.x != newCoordinates.x || oldCoordinates.y != newCoordinates.y) {
        const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(_entityLogicComponent.GetAngle());
        _entityRenderingComponent.PlayAnimation(AnimationType::RUNNING, direction, false);
    } else {
        const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(_entityLogicComponent.GetAngle());
        _entityRenderingComponent.PlayAnimation(AnimationType::IDLE, direction, false);
    }

}

RenderingContext Player::GetRenderingContext() {
    auto context = _entityRenderingComponent.GetRenderingContext();
    context.coordinates = _entityLogicComponent.GetCoordinates();
    return context;
}

void Player::Create(Server* server, int slotId) {
    auto player = std::make_shared<Player>(server, server->getSpawnPoint());
    server->addLocalPlayer(player);
    SaveManager::getInstance().setCurrentSlot(slotId);
}

void Player::Load(Server* server, int slotId) {
    // TODO: Load player data from save
    auto player = std::make_shared<Player>(server, server->getSpawnPoint());
    server->addLocalPlayer(player);

    // Load saved data into player
    SaveManager::getInstance().loadGame(slotId, server);
}
void Player::Save(Server* server) {
    int currentSlot = SaveManager::getInstance().getCurrentSlot();
    if (currentSlot >= 0) {
        SaveManager::getInstance().saveGame(currentSlot, server);
    }
}

void Player::SetId(const int newId) {
    id = newId;
}


void Player::SetCoordinates(const Coordinates &newCoordinates) {
    _entityLogicComponent.SetCoordinates(newCoordinates);
}

void Player::SetAngle(const int newAngle) {
    _entityLogicComponent.SetAngle(newAngle);
}

void Player::SetSpeed(float newSpeed) {
    _entityLogicComponent.SetSpeed(newSpeed);
}



void Player::SetEntityCollision(const bool disable) {
    _entityCollisionComponent.DisableCollision(disable);
}

void Player::AddEvent(std::unique_ptr<EntityEvent> eventData) {
    _entityLogicComponent.AddEvent(std::move(eventData));
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

HitboxContext Player::GetHitboxRenderingContext() const {
    auto context = _entityCollisionComponent.GetHitboxContext();
    context.coordinates = _entityLogicComponent.GetCoordinates();
    return context;
}

int Player::GetId() const {
    return id;
}

int Player::GetReach() const {
    return reach;
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

Player::Player(Server* server, const Coordinates& coordinates){
    _server = server;
    _entityLogicComponent.SetCoordinates(coordinates);
    _entityLogicComponent.SetSpeed(200.0f);
}
