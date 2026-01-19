//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Player.hpp"
#include "../../include/Application/SaveGame.h"
#include <memory>


//PlayerNew

void Player::Tick() {

    if (entityHealthComponent.IsDead() && !IsGhostMode() )  {
        SetGhostMode(true);
    }

    const auto deltaTime = server->GetDeltaTime_unprotected();
    const auto oldCoordinates = entityLogicComponent.GetCoordinates();
    if (beingRevived) ReviveFromGhostMode();
    if (isGhostMode) entityRenderingComponent2.Tick(deltaTime);
    else entityRenderingComponent.Tick(deltaTime);
    entityLogicComponent.Tick(server, *this);
    entityHealthComponent.Tick(deltaTime);
    const auto newCoordinates = entityLogicComponent.GetCoordinates();
    if (entityLogicComponent.IsInterrupted()) return;

    if (oldCoordinates != newCoordinates) {
        const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(entityLogicComponent.GetAngle());
        GetRenderingComponent()->PlayAnimation(AnimationType::RUNNING, direction, 1);
    } else {
        const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(entityLogicComponent.GetAngle());
        GetRenderingComponent()->PlayAnimation(AnimationType::IDLE, direction, 1);
    }
}

RenderingContext Player::GetRenderingContext() {
    if (isGhostMode) {
        auto context2 = entityRenderingComponent2.GetRenderingContext();
        context2.coordinates = entityLogicComponent.GetCoordinates();
        return context2;
    }
    auto context = entityRenderingComponent.GetRenderingContext();
    context.coordinates = entityLogicComponent.GetCoordinates();
    return context;
}

void Player::Create(Server* server, int slotId) {
    auto player = std::make_shared<Player>(server, server->GetSpawnPoint());
    server->AddLocalPlayer(player);
    SaveManager::getInstance().setCurrentSlot(slotId);
}

void Player::Load(Server* server, int slotId) {
    // TODO: Load player data from save
    auto player = std::make_shared<Player>(server, server->GetSpawnPoint());
    server->AddLocalPlayer(player);

    // Load saved data into player
    SaveManager::getInstance().loadGame(slotId, server);
}

void Player::Save(Server* server) {
    int currentSlot = SaveManager::getInstance().getCurrentSlot();
    if (currentSlot >= 0) {
        SaveManager::getInstance().saveGame(currentSlot, server);
    }
}

void Player::SetGhostMode(const bool enable) { isGhostMode = enable; }

void Player::ReviveFromGhostMode() {
    if (entityLogicComponent.IsLocked()) return;
    if (!beingRevived) {
        entityRenderingComponent2.PlayAnimation(AnimationType::DEATH, Direction::OMNI, 1, true);
        entityLogicComponent.SetLock();
        beingRevived = true;
        return;
    };
    isGhostMode = false;
    beingRevived = false;
    entityHealthComponent.SetHealth(entityHealthComponent.GetMaxHealth() / 2);
}

bool Player::IsBeingRevived() const {
    return beingRevived;
}

bool Player::IsGhostMode() const { return isGhostMode; }

void Player::SetId(const int newId) {
    id = newId;
}


void Player::SetCoordinates(const Coordinates &newCoordinates) {
    entityLogicComponent.SetCoordinates(newCoordinates);
}

void Player::SetAngle(const int newAngle) {
    entityLogicComponent.SetAngle(newAngle);
}

void Player::SetSpeed(const float newSpeed) {
    entityLogicComponent.SetSpeed(newSpeed);
}



void Player::SetEntityCollision(const bool disable) {
    entityCollisionComponent.DisableCollision(disable);
}

void Player::AddEvent(std::unique_ptr<EntityEvent> eventData) {
    entityLogicComponent.AddEvent(std::move(eventData));
}

void Player::SetPlayerUUID(const int newPlayerId) { uuid = newPlayerId; }

void Player::SetInventoryId(int newInventoryId) {
    inventoryId = newInventoryId;
}

Coordinates Player::GetCoordinates() const {
    return entityLogicComponent.GetCoordinates();
}

Coordinates Player::GetEntityCenter() {
    const auto adjustment{entityRenderingComponent.CalculateCenterOffset(*this)};
    auto coordinates {entityLogicComponent.GetCoordinates()};
    coordinates.x += adjustment.x;
    coordinates.y += adjustment.y;
    return coordinates;
}

CollisionStatus Player::GetCollisionStatus() const {
    return entityCollisionComponent.GetCollisionStatus();
}

int Player::GetAngle() const {
    return entityLogicComponent.GetAngle();
}

HitboxContext Player::GetHitboxRenderingContext() const {
    auto context = entityCollisionComponent.GetHitboxContext();
    context.coordinates = entityLogicComponent.GetCoordinates();
    return context;
}

int Player::GetId() const {
    return id;
}

int Player::GetReach() const {
    return reach;
}

float Player::GetSpeed() const {
    return entityLogicComponent.GetSpeed();
}

float Player::GetDetectionRange() const { return 0; }

float Player::GetAttackRange() const { return 0; }

EntityType Player::GetType() const {
    return EntityType::PLAYER;
}

int Player::GetPlayerUUID() const { return uuid; }

int Player::GetInventoryId() const {
    return inventoryId;
}

EntityCollisionComponent * Player::GetCollisionComponent() {
    return &entityCollisionComponent;
}

EntityLogicComponent * Player::GetLogicComponent() {
    return &entityLogicComponent;
}

EntityHealthComponent * Player::GetHealthComponent() {
    return &entityHealthComponent;
}

EntityRenderingComponent * Player::GetRenderingComponent() {
    if (isGhostMode) {
        return &entityRenderingComponent2;
    }
    return &entityRenderingComponent;
}

EntityInventoryComponent * Player::GetInventoryComponent() {
    return &entityInventoryComponent;
}

Server* Player::GetServer() const {
    return server;
}

Player::Player(Server* server, const Coordinates& coordinates){
    this->server = server;
    entityLogicComponent.SetCoordinates(coordinates);
    entityLogicComponent.SetSpeed(200.0f);
}
