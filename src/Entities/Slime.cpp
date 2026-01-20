//
// Created by Lukáš Kaplánek on 16.01.2026.
//
//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Slime.h"
#include "../../include/Items/Item.h"
#include "../../include/Server/Server.h"
#include <memory>
#include <random>
#include <SDL3/SDL_log.h>

#include "../../include/Sprites/SlimeSprite_Fire.h"
#include "../../include/Sprites/SlimeSprite_Ghost.h"


void Slime::Tick() {
    const auto deltaTime = server->GetDeltaTime_unprotected();
    const auto oldCoordinates = entityLogicComponent.GetCoordinates();
    entityRenderingComponent->Tick(deltaTime);
    entityLogicComponent.Tick(server, *this);
    entityHealthComponent->Tick(deltaTime);
    const auto newCoordinates = entityLogicComponent.GetCoordinates();
    if (entityLogicComponent.IsInterrupted()) return;

    if (oldCoordinates != newCoordinates) {
        const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(entityLogicComponent.GetAngle());
        entityRenderingComponent->PlayAnimation(AnimationType::RUNNING, direction, 1);
    } else {
        const auto direction = EntityRenderingComponent::GetDirectionBaseOnAngle(entityLogicComponent.GetAngle());
        entityRenderingComponent->PlayAnimation(AnimationType::IDLE, direction, 1);
    }

}

RenderingContext Slime::GetRenderingContext() {
    auto context = entityRenderingComponent->GetRenderingContext();
    context.coordinates = entityLogicComponent.GetCoordinates();
    return context;
}

void Slime::SetId(const int newId) {
    id = newId;
}


void Slime::SetCoordinates(const Coordinates &newCoordinates) {
    entityLogicComponent.SetCoordinates(newCoordinates);
}


void Slime::SetAngle(const int newAngle) {
    entityLogicComponent.SetAngle(newAngle);
}

void Slime::SetSpeed(float newSpeed) {
    entityLogicComponent.SetSpeed(newSpeed);
}



void Slime::SetEntityCollision(const bool disable) {
    entityCollisionComponent.DisableCollision(disable);
}

void Slime::AddEvent(std::unique_ptr<EntityEvent> eventData) {
    entityLogicComponent.AddEvent(std::move(eventData));
}

Coordinates Slime::GetCoordinates() const {
    return entityLogicComponent.GetCoordinates();
}

Coordinates Slime::GetEntityCenter() {
    const auto adjustment{entityRenderingComponent->CalculateCenterOffset(*this)};
    auto coordinates {entityLogicComponent.GetCoordinates()};
    coordinates.x += adjustment.x;
    coordinates.y += adjustment.y;
    return coordinates;
}

CollisionStatus Slime::GetCollisionStatus() const {
    return entityCollisionComponent.GetCollisionStatus();
}

int Slime::GetAngle() const {
    return entityLogicComponent.GetAngle();
}

HitboxContext Slime::GetHitboxRenderingContext() const {
    auto context = entityCollisionComponent.GetHitboxContext();
    context.coordinates = entityLogicComponent.GetCoordinates();
    return context;
}

int Slime::GetId() const {
    return id;
}

int Slime::GetReach() const {
    return reach;
}

float Slime::GetSpeed() const {
    return entityLogicComponent.GetSpeed();
}

float Slime::GetDetectionRange() const {
    return detectionRange;
}

float Slime::GetAttackRange() const {
    return attackRange;
}

EntityType Slime::GetType() const {
    return EntityType::SLIME;
}

int Slime::GetVariant() const { return variant; }

void Slime::DropItemsOnDeath() {
    auto dropItem = [&](Server* server, std::unique_ptr<Item> Item) {
        bool added = server->AddItemToInventory(std::move(Item));
        SDL_Log("Item added to UI inventory: %s", added ? "true" : "false");
    };
    std::mt19937 generator(server->GetSeed());
    std::uniform_int_distribution distribution(1, 100);
    const int roll = distribution(generator);

    SDL_Log("Slime gone");
    std::unique_ptr<Item> Item;
    switch (variant) {
        case 2:
            dropItem(server, ItemFactory::createMaterial(MaterialType::LEATHER));
            (roll > 5) ? dropItem(server, ItemFactory::createMaterial(MaterialType::IRON))
            : dropItem(server, ItemFactory::createMaterial(MaterialType::GOLD));
            break;
        case 3:
            dropItem(server, ItemFactory::createMaterial(MaterialType::LEATHER));
            if (roll <= 3)
                dropItem(server, ItemFactory::createMaterial(MaterialType::DRAGONSCALE));
            else if (roll <= 13)
                dropItem(server, ItemFactory::createMaterial(MaterialType::STEEL));
            else
                dropItem(server, ItemFactory::createMaterial(MaterialType::GOLD));
            break;
        default:
            dropItem(server, ItemFactory::createMaterial(MaterialType::LEATHER));
            break;
    }
}
//regular slime: leather
//medium slime: leather + iron + gold(5%)
//large slime: leather + gold + steel(10%) + dragonscale(3%)

EntityCollisionComponent * Slime::GetCollisionComponent() {
    return &entityCollisionComponent;
}

EntityLogicComponent * Slime::GetLogicComponent() {
    return &entityLogicComponent;
}

EntityHealthComponent * Slime::GetHealthComponent() {
    return entityHealthComponent.get();
}

EntityRenderingComponent * Slime::GetRenderingComponent() {
    return entityRenderingComponent.get();
}

EntityInventoryComponent * Slime::GetInventoryComponent() {
    return &entityInventoryComponent;
}

Server* Slime::GetServer() const {
    return server;
}

Slime::Slime(Server* server, const Coordinates& coordinates, const int variant) : variant(variant) {
    this->server = server;
    entityLogicComponent.SetCoordinates(coordinates);
    std::mt19937 generator(server->GetSeed());
    std::uniform_real_distribution distribution(60.0f, 80.0f);
    entityLogicComponent.SetSpeed(distribution(generator));

    if (variant == 2) {
        entityHealthComponent = std::make_unique<EntityHealthComponent>(30,30);
        entityRenderingComponent= std::make_unique<EntityRenderingComponent>(std::make_unique<SlimeSprite_Ghost>());
        return;
    }
    if (variant == 3) {
        entityHealthComponent = std::make_unique<EntityHealthComponent>(50,50);
        entityRenderingComponent= std::make_unique<EntityRenderingComponent>(std::make_unique<SlimeSprite_Fire>());
        return;
    }
    entityHealthComponent = std::make_unique<EntityHealthComponent>(20,20);
    entityRenderingComponent= std::make_unique<EntityRenderingComponent>(std::make_unique<SlimeSprite>());
}
