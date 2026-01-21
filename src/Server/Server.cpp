//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#include "../../include/Server/Server.h"

#include <filesystem>
#include <fstream>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <simdjson.h>


#include "../../include/Application/MACROS.h"
#include "../../include/Items/Item.h"
#include "../../include/Application/SaveGame.h"
#include "../../include/Server/generace_mapy.h"
#include "../../include/Entities/Entity.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Entities/Slime.h"
#include "../../include/Items/inventory.h"
#include "../../include/Structures/Anchor.h"
#include "../../include/Structures/Chest.h"
#include "../../include/Structures/OreNode.h"
#include "../../include/Structures/Structure.h"
#include "../../include/Structures/Tree.h"
#include "../../include/Window/WorldStructs.h"
#include "../../include/Structures/OreDeposit.h"

void Server::SetEntityPos(int entityId, Coordinates newCoordinates) {
    std::lock_guard lock(serverMutex);
    if (entities.contains(entityId)) {
        entities[entityId]->GetCoordinates() = newCoordinates;
    }
}

void Server::SetEntityCollision(int entityId, bool disable) {
    std::lock_guard lock(serverMutex);
    if (entities.contains(entityId)) {
        entities[entityId]->SetEntityCollision(disable);
    }
}


bool Server::IsEntityColliding(int entityId) {
    std::shared_lock lock(serverMutex);
    if (entities.contains(entityId)) {
        return entities[entityId]->GetCollisionStatus().colliding;
    }
    return false; // Return false if entity not found
}

int Server::getNextEntityId() {return nextEntityId++;}

int Server::getNextStructureId() {
    if (!reclaimedStructureIds.empty()) {
        const int id = reclaimedStructureIds.back();
        reclaimedStructureIds.pop_back();
        return id;
    }
    return ++nextStructureId;
}

void Server::setupSlimeAi(IEntity* entity) {
    aiManager.registerEntity(entity);
    auto* sm = aiManager.getStateMachine(entity);

    // Registrace handlerů pro stavy
    sm->registerState(AiState::Idle,
        [](IEntity* entityInstance, float dt) {
            entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE) );
            entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO) );
        },
        [](IEntity* entityInstance, float dt) {
            if (entityInstance->GetServer()->aiManager.getTimeInState(entityInstance) >= 5.0f) { // Po 5 sekundách přepni do Patrol
                entityInstance->GetServer()->aiManager.sendEvent(entityInstance, AiEvent::ReachedDestination);
            }
        }
    );

    sm->registerState(AiState::Patrol,
    [](IEntity* entityInstance, float dt) {
        std::mt19937 rng(static_cast<unsigned int>(SDL_GetTicks()) + entityInstance->GetId());
        std::uniform_int_distribution<int> dist(-100, 100);

        const auto coords {entityInstance->GetCoordinates()};
        entityInstance->GetLogicComponent()->AddEvent(Event_MoveTo::Create(coords.x+static_cast<float>(dist(rng)),coords.y+static_cast<float>(dist(rng))));

        entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE) );
        entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO) );
    },
    [](IEntity* entityInstance, float dt) {
        if (entityInstance->GetServer()->aiManager.getTimeInState(entityInstance) >= 5.0f) {
            entityInstance->GetServer()->aiManager.sendEvent(entityInstance, AiEvent::ReachedDestination);
        }
    });

    sm->registerState(AiState::Chase,
        [](IEntity* entityInstance, float dt) {
            entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE) );
            entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO) );
        },
        [](IEntity* entityInstance, float dt) {
            if (const auto localPlayer{entityInstance->GetServer()->localPlayer}) {
            const auto dx{localPlayer->GetEntityCenter().x - entityInstance->GetEntityCenter().x};
            const auto dy {localPlayer->GetEntityCenter().y - entityInstance->GetEntityCenter().y};
            entityInstance->GetLogicComponent()->AddEvent(Event_Move::Create(dx,dy));
            }
        }
    );


    sm->registerState(AiState::Attack,
        [](IEntity* entityInstance, float dt) {
            entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE) );
            entityInstance->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO) );
        },
        [](IEntity* entityInstance, float dt) {
            const auto logicComp{entityInstance->GetLogicComponent()};
            logicComp->AddEvent(Event_SetAngle::Create(CalculateAngle(entityInstance->GetEntityCenter(), entityInstance->GetServer()->GetPlayer_unprotected()->GetEntityCenter())));
            logicComp->PerformAttack(entityInstance,1,2+ entityInstance->GetVariant()*3);
            //logicComp->AddEvent(Event_Attack::Create(1,5));
        }
    );

    sm->registerState(AiState::GetUnstuck,
        [](IEntity* entity, float dt) {
            const auto [x, y]{entity->GetServer()->GetPlayer_unprotected()->GetEntityCenter()};
            entity->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE),true );
            entity->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO),true );
            entity->GetLogicComponent()->AddEvent(Event_MoveTo::Create(x,y),true);
        },
        [](IEntity* entity, float dt) {
            const auto logicComp{entity->GetLogicComponent()};
            const auto distance = CoordinatesDistance(logicComp->GetCoordinates(),logicComp->GetLastMoveDirection());
            if (std::abs(distance) <  EntityLogicComponent::threshold) {
                entity->GetServer()->aiManager.sendEvent(entity, AiEvent::MovementStuck);
            }
        }
    );

    // Registrace prechodu
    sm->registerTransition(AiState::Idle, AiEvent::ReachedDestination, AiState::Patrol);
    sm->registerTransition(AiState::Idle, AiEvent::PlayerSpotted, AiState::Chase);
    sm->registerTransition(AiState::Idle, AiEvent::TookDamage, AiState::Chase);

    sm->registerTransition(AiState::Chase, AiEvent::TargetInRange, AiState::Attack);
    sm->registerTransition(AiState::Chase, AiEvent::PlayerLost, AiState::Idle);
    sm->registerTransition(AiState::Chase, AiEvent::MovementStuck, AiState::GetUnstuck);

    sm->registerTransition(AiState::Attack, AiEvent::TargetOutOfRange, AiState::Chase);
    sm->registerTransition(AiState::Attack, AiEvent::PlayerLost, AiState::Idle);

    sm->registerTransition(AiState::GetUnstuck, AiEvent::ReachedDestination, AiState::Chase);
    sm->registerTransition(AiState::GetUnstuck, AiEvent::MovementStuck, AiState::GetUnstuck);
    sm->registerTransition(AiState::GetUnstuck, AiEvent::TargetInRange, AiState::Attack);

    sm->registerTransition(AiState::Patrol, AiEvent::ReachedDestination, AiState::Patrol);
    sm->registerTransition(AiState::Patrol, AiEvent::TookDamage, AiState::Chase);
    sm->registerTransition(AiState::Patrol, AiEvent::PlayerSpotted, AiState::Chase);
}


bool Server::check3by3AreaFree(int x, int y) const {
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (GetMapValue_unprotected(x + dx, y + dy) != 0) return false;
        }
    }
    return true;
}

void Server::SetSpawnPoint(Coordinates newSpawnPoint) {spawnPoint = newSpawnPoint;}

void Server::SetDeltaTime(float dt) {
    std::lock_guard lock(serverMutex);
    deltaTime = dt;
}

float Server::GetDeltaTime(){
    std::shared_lock lock(serverMutex);
    return deltaTime;
}
float Server::GetDeltaTime_unprotected() const{
    return deltaTime;
}

int Server::GetMapValue(int x, int y, WorldData::MapType mapType) {
    if (x < 0 || y < 0 || x >= MAPSIZE || y >= MAPSIZE) {
        return -1; // Out of bounds
    }
    std::shared_lock lock(serverMutex);
    return worldData.GetMapValue(x,y, mapType);
}

int Server::GetMapValue_unprotected(int x, int y, WorldData::MapType mapType) const {
    if (x < 0 || y < 0 || x >= MAPSIZE || y >= MAPSIZE) {
        return -1; // Out of bounds
    }
    return worldData.GetMapValue(x,y, mapType);
}


void Server::SetMapValue(int x, int y, WorldData::MapType mapType, int value) {
    std::lock_guard lock(serverMutex);
    worldData.UpdateMapValue(x,y, mapType, value);
}

void Server::SetMapValue_unprotected(int x, int y, WorldData::MapType mapType, int value) {
    worldData.UpdateMapValue(x,y, mapType, value);
}

void Server::AddLocalPlayer(const std::shared_ptr<Player>& player) {
    std::lock_guard lock(serverMutex);
    localPlayer = player;
    entities[0] = player;
    player->SetId(0); //Local player always has ID 0
}

IEntity* Server::AddEntity_unprotected(const std::shared_ptr<IEntity>& entity) {
    const int newId = getNextEntityId();
    entities.insert_or_assign(newId, entity);
    entity->SetId(newId);
    return entity.get();
}

void Server::AddEntity(const std::shared_ptr<IEntity>& entity) {
    std::lock_guard lock(serverMutex);
    const int newId = getNextEntityId();
    entities.insert_or_assign(newId, entity);
    entity->SetId(newId);
}


void Server::AddStructure(Coordinates coordinates, structureType type, int innerType, int variant) {
    std::lock_guard lock(serverMutex);
    AddStructure_unprotected(coordinates, type, innerType, variant);
}

bool Server::AddStructure_unprotected(Coordinates coordinates, const structureType type, int innerType, int variant) {
    int newId = getNextStructureId();
    cacheValidityData.isCacheValid = false; //Invalidae cache

    std::shared_ptr<IStructure> newStructure;
    switch (type) {
        case structureType::TREE: {
            newStructure = std::make_shared<Tree>(newId, coordinates, GetSharedPtr(), innerType);
            break;
        }
        case structureType::ORE_NODE: {
            newStructure = std::make_shared<OreNode>(newId, coordinates, GetSharedPtr(), innerType, variant);
            break;
        }
        case structureType::ORE_DEPOSIT: {
            newStructure = std::make_shared<OreDeposit>(newId, coordinates, GetSharedPtr(), innerType, variant);
            break;
        }
        case structureType::CHEST: {
            newStructure = std::make_shared<Chest>(newId, coordinates, GetSharedPtr());
            break;
        }
        case structureType::RESPAWN_ANCHOR: {
            newStructure = std::make_shared<Anchor>(newId, coordinates, GetSharedPtr());
            break;
        }
        default:
            break;
    }

    if (!newStructure || !newStructure->WasProperlyInitialized()) {
        SDL_Log("Failed to create Tree structure at (%f, %f)", coordinates.x, coordinates.y);
        nextStructureId--; //Rollback ID pokud se nepovede vytvorit struktura
        return false;
    }
    structures[newId] =newStructure;
    return true;
}


void Server::RemoveEntity(int entityId) {
    std::lock_guard lock(serverMutex);
    if (!entities.contains(entityId)) return;
    entitiesToRemove.emplace_back(entityId);
}

void Server::RemoveEntity_unprotected(int entityId) {
    if (!entities.contains(entityId)) return;
    entitiesToRemove.emplace_back(entityId);
}

void Server::RemoveStructure(int structureId) {
    std::lock_guard lock(serverMutex);
    if (!structures.contains(structureId)) return;
    structures.erase(structureId);
    reclaimedStructureIds.emplace_back(structureId);
    cacheValidityData.isCacheValid = false; //Invalidae cache
}

StructureData Server::GetStructureData(const IStructure *structure) {
    StructureData data;
    data.type = static_cast<int>(structure->GetType());
    data.innerType = structure->GetInnerType();
    data.variant = structure->GetVariant();
    data.inventoryId = structure->GetInventoryId();
    const auto coords{structure->GetCoordinates()};
    data.x = static_cast<int>(coords.x);
    data.y = static_cast<int>(coords.y);
    return data;
}

EntityData Server::GetEntityData(IEntity *entity) {
    EntityData data;
    data.type = static_cast<int>(entity->GetType());
    data.health = entity->GetHealthComponent()->GetHealth();
    const auto coords{entity->GetCoordinates()};
    data.x = coords.x;
    data.y = coords.y;
    data.variant = entity->GetVariant();
    return data;
}



void Server::SaveServerState() {
    if (serverState != ServerState::RUNNING) {
        SDL_Log("Server is not running, cannot save state.");
        return;
    }
    std::vector<StructureData> structuresData;
    structuresData.reserve(structures.size());
    {
        std::shared_lock lock(serverMutex);
        for (const auto &structure: structures | std::views::values) {
            structuresData.emplace_back(GetStructureData(structure.get()));
        }
    }
    structuresData.shrink_to_fit();

    std::vector<EntityData> entitiesData;
    entitiesData.reserve(entities.size());
    {
        std::shared_lock lock(serverMutex);
        for (const auto &entity: entities | std::views::values) {
            if (entity->GetType() == EntityType::PLAYER) continue;
            entitiesData.emplace_back(GetEntityData(entity.get()));
        }
    }
    entitiesData.shrink_to_fit();

    std::vector<Coordinates> waterTiles = worldData.GetWaterTiles();
    waterTiles.shrink_to_fit();

    simdjson::builder::string_builder sb;

    sb.start_object();
    sb.append("waterTiles");
    sb.append_colon();
    sb.start_array();
    for (size_t i = 0; i < waterTiles.size(); ++i) {
        sb.start_object();
        sb.append_key_value("x", static_cast<int>(waterTiles.at(i).x));
        sb.append_comma();
        sb.append_key_value("y", static_cast<int>(waterTiles.at(i).y));
        sb.end_object();
        if (i + 1 < waterTiles.size()) sb.append_comma();
    }
    sb.end_array();
    sb.append_comma();

    sb.append("structures");
    sb.append_colon();
    sb.start_array();
    for (size_t i = 0; i < structuresData.size(); ++i) {
         const auto type = StructureRenderingComponent::TypeToString(static_cast<structureType>(structuresData.at(i).type));
        sb.start_object();
        sb.append_key_value("type", structuresData.at(i).type);
        sb.append_comma();
        sb.append_key_value("variant", structuresData.at(i).innerType);
        sb.append_comma();
        sb.append_key_value("innerType", structuresData.at(i).variant);
        sb.append_comma();
        sb.append_key_value("inventoryId", structuresData.at(i).inventoryId);
        sb.append_comma();
        sb.append_key_value("x", structuresData.at(i).x);
        sb.append_comma();
        sb.append_key_value("y", structuresData.at(i).y);
        sb.end_object();
        if (i + 1 < structuresData.size()) sb.append_comma();
    }
    sb.end_array();
    sb.append_comma();

    sb.append("entities");
    sb.append_colon();
    sb.start_array();
    for (size_t i = 0; i < entitiesData.size(); ++i) {
        sb.start_object();
        sb.append_key_value("type", entitiesData.at(i).type);
        sb.append_comma();
        sb.append_key_value("health", entitiesData.at(i).health);
        sb.append_comma();
        sb.append_key_value("variant", entitiesData.at(i).variant);
        sb.append_comma();
        sb.append_key_value("x", entitiesData.at(i).x);
        sb.append_comma();
        sb.append_key_value("y", entitiesData.at(i).y);
        sb.end_object();
        if (i + 1 < entitiesData.size()) sb.append_comma();
    }
    sb.end_array();
    sb.end_object();

    std::fstream file("saves/slot_" + std::to_string(SaveManager::getInstance().getCurrentSlot()) + "_server_state.json", std::ios::out);
    file << sb.c_str();
    file.close();

    // Save inventory through callback
    if (onSaveInventory) {
        std::string inventoryPath = "saves/slot_" + std::to_string(SaveManager::getInstance().getCurrentSlot()) + "_inventory.json";
        onSaveInventory(inventoryPath);
    }
}

void Server::LoadServerState() {
    serverState = ServerState::RUNNING;
    const auto fileName{"saves/slot_" + std::to_string(SaveManager::getInstance().getCurrentSlot()) + "_server_state.json"};
    if (!std::filesystem::exists(fileName)) {
        SDL_Log("Save file does not exist: %s", fileName.c_str());
        return;
    }
    const auto jsonContent{simdjson::padded_string::load(fileName)};
    simdjson::ondemand::parser parser;
    auto doc{parser.iterate(jsonContent)};

    for (auto waterTileData : doc["waterTiles"].get_array()) {
        auto obj = waterTileData.get_object();
        const int x{static_cast<int>(obj["x"].get_int64())};
        const int y{static_cast<int>(obj["y"].get_int64())};
        worldData.UpdateMapValue(x, y, WorldData::COLLISION_MAP, -1);
    }

    for (auto structureData : doc["structures"].get_array()) {
        auto obj = structureData.get_object();

        // Přečíst všechny hodnoty najednou do lokálních proměnných
        double x = obj["x"].get_double();
        double y = obj["y"].get_double();
        int type = static_cast<int>(obj["type"].get_int64());
        int innerType = static_cast<int>(obj["innerType"].get_int64());
        int variant = static_cast<int>(obj["variant"].get_int64());

        Coordinates coords{static_cast<float>(x), static_cast<float>(y)};

        AddStructure(coords, static_cast<structureType>(type), innerType, variant);
    }

    for (auto entityData : doc["entities"].get_array()) {
        auto obj = entityData.get_object();
        Coordinates coords{static_cast<float>(obj["x"].get_double()), static_cast<float>(obj["y"].get_double())};
        const auto entityType{static_cast<EntityType>(static_cast<int>(obj["type"].get_int64()))};
        const int health {static_cast<int>(obj["health"].get_int64())};
        const int variant {static_cast<int>(obj["variant"].get_int64())};
        AddEntity(coords, entityType,variant)->GetHealthComponent()->SetHealth(health);
    }

    // Load inventory through callback
    if (onLoadInventory) {
        std::string inventoryPath = "saves/slot_" + std::to_string(SaveManager::getInstance().getCurrentSlot()) + "_inventory.json";
        if (std::filesystem::exists(inventoryPath)) {
            onLoadInventory(inventoryPath);
        }
    }
}

void Server::Reset() {
    entities.clear();
    structures.clear();
    nextEntityId = 1;
    nextStructureId = 0;
    reclaimedStructureIds.clear();
    structuresToRemove.clear();
    entitiesToRemove.clear();
    localPlayer = nullptr;
    damagePoints.clear();
    aiManager.unregisterAllEntities();
    deltaTime = 0.0f;
    lastDamagePoints.clear();
    cacheValidityData.isCacheValid = false;
    StructureIdCache.clear();
    worldData.ResetMaps();
}

void Server::SetServerState(const ServerState newState) {
    std::lock_guard lock(serverMutex);
    serverState = newState;
}

void Server::PlayerUpdate(std::unique_ptr<EntityEvent> e, int playerId) {
    std::lock_guard lock(serverMutex);
    if (localPlayer) localPlayer->AddEvent(std::move(e));
}

std::set<int> Server::GetStructuresInArea(Coordinates topLeft, Coordinates bottomRight) {
    std::lock_guard lock(serverMutex);
    if (std::abs(topLeft.x - cacheValidityData.lastPlayerPos.x) >= static_cast<float>(cacheValidityData.rangeForCacheUpdate)
        ||std::abs(topLeft.y - cacheValidityData.lastPlayerPos.y) >= static_cast<float>(cacheValidityData.rangeForCacheUpdate))
        cacheValidityData.isCacheValid = false;


    if (cacheValidityData.isCacheValid) {
        return StructureIdCache;
    }

    SDL_Log("Refreshing structure ID cache");
    cacheValidityData.lastPlayerPos = topLeft;
    StructureIdCache.clear();
    const int rangeXMin = static_cast<int>(std::floor((topLeft.x) / 32.0f)) - cacheValidityData.rangeForCacheUpdate/32;
    const int rangeYMin = static_cast<int>(std::floor((topLeft.y) / 32.0f)) - cacheValidityData.rangeForCacheUpdate/32;
    const int rangeXMax = static_cast<int>(std::ceil((bottomRight.x) / 32.0f)) + cacheValidityData.rangeForCacheUpdate/32;
    const int rangeYMax = static_cast<int>(std::ceil((bottomRight.y) / 32.0f)) + cacheValidityData.rangeForCacheUpdate/32;

    for (int x = rangeXMin; x <= rangeXMax; x++) {
        if (x < 0 || x >= MAPSIZE) continue;
        for (int y = rangeYMin; y <= rangeYMax; y++) {
            if (y < 0 || y >= MAPSIZE) continue;
            if (int id = GetMapValue_unprotected(x, y, WorldData::COLLISION_MAP); id > 0)  StructureIdCache.emplace(id);
        }
    }
    cacheValidityData.isCacheValid = true;
    return StructureIdCache;
}

void Server::ApplyDamageAt_unprotected(const int damage, const std::vector<Coordinates>& positions, const int entityId) {
    for (const auto& position : positions) {
        damagePoints.emplace_back(DamageArea{
            .coordinates = position,
            .damage = damage,
            .entityId = entityId
        });
    }
}

int Server::CalculateAngle(Coordinates center, Coordinates point) {
        auto angle = static_cast<int>(std::atan2(point.x - center.x,
                                                 point.y - center.y) * 180.0f / M_PI);
        if (angle < 0) angle += 360;
        return angle;
}

std::vector<std::string> Server::GetTileInfo(const float x, const float y) {
    std::shared_lock lock(serverMutex);
    const int tileX{static_cast<int>(std::floor(x / 32.0f))};
    const int tileY{static_cast<int>(std::floor(y / 32.0f))};
    const auto mapValue{worldData.GetMapValue(tileX, tileY, WorldData::COLLISION_MAP)};

     std::vector<std::string> text;

    for (const auto &entity: entities | std::views::values) {
        const auto entityPos{entity->GetEntityCenter()};
        const int entityTileX{static_cast<int>(std::floor(entityPos.x / 32.0f))};
        const int entityTileY{static_cast<int>(std::floor(entityPos.y / 32.0f))};
        if (entityTileX == tileX && entityTileY == tileY) {
            text.emplace_back(EntityRenderingComponent::TypeToString(entity->GetType()));
        }
    }

    if ( mapValue> 0) text.emplace_back(StructureRenderingComponent::TypeToString(GetStructure(mapValue)->GetType()));

    return text;
}

void Server::InvalidateStructureCache() {
    std::lock_guard lock(serverMutex);
    cacheValidityData.isCacheValid = false;
}

void Server::SendClickEvent(const MouseButtonEvent &event) const {

    auto checkForStructure= [](const IEntity* player, const MouseButtonEvent &eventData) {
        const auto [x, y]{toTileCoordinates(static_cast<int>(eventData.x), static_cast<int>(eventData.y))};
        return player->GetServer()->GetMapValue_unprotected(static_cast<int>(x),static_cast<int>(y),WorldData::COLLISION_MAP);
    };

    auto sendAttack = [](IEntity* player, const MouseButtonEvent &eventData) {
        const auto logicComp{player->GetLogicComponent()};
        logicComp->AddEvent(Event_SetAngle::Create(CalculateAngle(player->GetEntityCenter(), Coordinates{eventData.x, eventData.y})));
        logicComp->AddEvent(Event_ClickAttack::Create(2,10,{eventData.x, eventData.y}));

        };

    auto sendMine = [](IEntity* player, const MouseButtonEvent &eventData) {
        const auto logicComp{player->GetLogicComponent()};
        const auto server{player->GetServer()};
        if (!eventData.sameTile) server->mineProgress == 0.0f;
        logicComp->AddEvent(Event_SetAngle::Create(CalculateAngle(player->GetEntityCenter(), Coordinates{eventData.x, eventData.y})));
        const auto tile{toTileCoordinates(static_cast<int>(eventData.x), static_cast<int>(eventData.y))};
        const auto id = server->GetMapValue_unprotected(
            static_cast<int>(tile.x),
            static_cast<int>(tile.y),
            WorldData::COLLISION_MAP);
        if (id <= 0) return;
        const auto structure{server->GetStructure(id)};
        if (!structure) return;

        if (const auto type{structure->GetType()};
            type != structureType::TREE
            && type != structureType::ORE_DEPOSIT
            && type != structureType::ORE_NODE
            && type != structureType::CHEST) return;

        server->mineProgress += 0.1f; //TODO: pridat rychlost podle nastroje
        if (server->mineProgress >= 1.0f) {
            server->mineProgress = 0.0f;
            server->GetStructure(id)->DropInventoryItems();
            server->RemoveStructure(id);
        }
    };

    auto sendMoveTo = [](IEntity* player, const MouseButtonEvent event) {
        const auto logicComp{player->GetLogicComponent()};
        logicComp->AddEvent(Event_MoveTo::Create(event.x, event.y));
    };

    auto placeChest = [](const IEntity* player, const MouseButtonEvent &eventData) {
        const auto structure{player->GetServer()->AddStructure_unprotected(
            toWorldCoordinates(toTileCoordinates(static_cast<int>(eventData.x), static_cast<int>(eventData.y))),
            structureType::CHEST, 0, 0)};
        if (!structure) return false;
        return true;
    };

    auto interact = [&](IEntity* player, const MouseButtonEvent &eventData) -> bool {
        const auto structure{player->GetServer()->GetStructure(checkForStructure(player, eventData))};
        if (!structure) return false;
        structure->Interact(player);
        return true;
    };
    const auto tile {toTileCoordinates(event.x, event.y)};
    const auto tileID{GetMapValue_unprotected(static_cast<int>(tile.x),static_cast<int>(tile.y),WorldData::COLLISION_MAP)};

    if (event.button == MouseButtonEvent::Button::LEFT) {
        if (event.action == MouseButtonEvent::Action::PRESS) sendAttack(localPlayer.get(), event);
        if (event.action == MouseButtonEvent::Action::RELEASE) sendMine(localPlayer.get(), event);
    }
    else if (event.button == MouseButtonEvent::Button::RIGHT) {
        if (event.action == MouseButtonEvent::Action::RELEASE) return;
        if (tileID == 0) placeChest(localPlayer.get(), event);
        else if (tileID > 0) interact(localPlayer.get(), event);
        else sendMoveTo(localPlayer.get(), event);
    }
}

void Server::KillAllEntities() {
    std::lock_guard lock(serverMutex);

    auto killEntity = [this](const std::pair<const int, std::shared_ptr<IEntity>> &entityPair) {
        if (std::get<1>(entityPair)->GetType() == EntityType::PLAYER) return;
        const auto id{std::get<0>(entityPair)};
        entitiesToRemove.emplace_back(id);
        SDL_Log("Killed entity ID %d", id);
    };
    std::ranges::for_each(entities, killEntity);
}


void Server::SpawnRespawnAnchors() {
    std::lock_guard lock(serverMutex);
    const Coordinates spawnPoint{GetSpawnPoint()};
    std::mt19937 rng(static_cast<unsigned int>(SDL_GetTicks()));
    std::uniform_int_distribution<int> dist(-5000, 5000);
    do{
        Coordinates anchorPos{spawnPoint.x + static_cast<float>(dist(rng)), spawnPoint.y + static_cast<float>(dist(rng))};
        if (AddStructure_unprotected(anchorPos, structureType::RESPAWN_ANCHOR, 0, 0))
            SDL_Log("Spawn respawn anchor at (%f, %f)", anchorPos.x, anchorPos.y);
    } while (respawnPoints.size() < 5);
}

IEntity* Server::AddEntity(const Coordinates coordinates, const EntityType type, const int variant)
{
    std::unique_lock lock(serverMutex);
    return AddEntity_unprotected(coordinates, type, variant);
}

IEntity* Server::AddEntity_unprotected(Coordinates coordinates, const EntityType type, const int variant)
{
    IEntity* entity{nullptr};
    switch (type) {
        case EntityType::SLIME: {
            const auto slime{std::make_shared<Slime>(GetSharedPtr().get(), coordinates, variant)};
            entity = AddEntity_unprotected(slime);
            if (slime) setupSlimeAi(slime.get());
            break;
        }
        case EntityType::PLAYER: {
            const auto player{std::make_shared<Player>(GetSharedPtr().get(), coordinates)};
            entity = AddEntity_unprotected(player);
            break;
        }
        default:
            break;
    }
    return entity;
}


void Server::Tick() {

    auto tryToSpawnSlime = [&]() {
        constexpr auto maxAttempts{10};
        if (spawnGen.dist(spawnGen.generator) >5) return;

        Coordinates spawnCoordinates{};
        Coordinates playerCoordinates{localPlayer->GetCoordinates()};
        auto attempts{0};

        for (int i = 0; i < maxAttempts; i++) {
            attempts++;
            const auto x = playerCoordinates.x + static_cast<float>(spawnGen.dist(spawnGen.generator));
            const auto y = playerCoordinates.y + static_cast<float>(spawnGen.dist(spawnGen.generator));

            if (check3by3AreaFree(std::floor(x/32),std::floor(x/32))) continue;
            spawnCoordinates = {x, y};
            attempts--;
            break;
        }
        if (attempts >=maxAttempts) return;
        AddEntity_unprotected(spawnCoordinates, EntityType::SLIME,  spawnGen.distVariant(spawnGen.generator));
        SDL_Log("Spawned slime at (%f, %f)", spawnCoordinates.x, spawnCoordinates.y);
    };

    auto removeFarEntity = [this](IEntity* entity)->bool {
        if (CoordinatesDistance(localPlayer->GetEntityCenter(), entity->GetEntityCenter()) < 2000.0f)
            return false;
        RemoveEntity_unprotected(entity->GetId());
        SDL_Log("Removed entity ID %d for being too far", entity->GetId());
        return true;
    };

    auto playerDetection = [this](IEntity* player, IEntity* entity) {
        if (entity->GetType() == EntityType::PLAYER)  return;
        if (const auto distanceToPlayer {CoordinatesDistance(entity->GetEntityCenter(), player->GetEntityCenter())}; distanceToPlayer < entity->GetDetectionRange()
        && !player->GetHealthComponent()->IsDead()) {
            aiManager.sendEvent(entity, AiEvent::PlayerSpotted);
            if (distanceToPlayer < entity->GetAttackRange()) aiManager.sendEvent(entity, AiEvent::TargetInRange);
            else aiManager.sendEvent(entity, AiEvent::TargetOutOfRange);
        }
        else aiManager.sendEvent(entity, AiEvent::PlayerLost);
    };
    auto checkDamage = [this](const std::shared_ptr<IEntity>& entity) ->int {
        if (entity->GetHealthComponent()->IsDead()) return 0;
        std::set<int> appliedDamageIds{};
        int totalDamage{0};
        for (const auto& damageArea : damagePoints) {
            const auto damageEntity{GetEntity_unprotected(damageArea.entityId)};
            if (entity->GetId() == damageEntity->GetId()) continue; //Entity cannot damage itself
            if (entity->GetType() != EntityType::PLAYER && damageEntity->GetType() != EntityType::PLAYER) continue; //Only player can damage other entities
            if (!entity->GetCollisionComponent()->CheckPoint(damageArea.coordinates, *entity)) continue;
            if (appliedDamageIds.contains(damageArea.entityId)) continue;
            totalDamage += damageArea.damage;
            appliedDamageIds.insert(damageArea.entityId);
        }
        return totalDamage;
    };

    std::lock_guard lock(serverMutex);


    for (const auto& entityId : entitiesToRemove) {
        aiManager.unregisterEntity(GetEntity_unprotected(entityId));
        entities.erase(entityId);
        reclaimedEntityIds.emplace_back(entityId);
    }
    entitiesToRemove.clear();

    for (const auto& structureId : structuresToRemove) {
        structures.erase(structureId);
        reclaimedStructureIds.emplace_back(structureId);
    }
    structuresToRemove.clear();

    aiManager.update(deltaTime);
    for (const auto &entity: entities | std::views::values) {
        if (!entity) continue;
        if (removeFarEntity(entity.get())) continue;
        entity->Tick();
        playerDetection(localPlayer.get(), entity.get()); //Detekce hrace pro AI, zatim pouze lokalni hrac
        if (const auto damage{checkDamage(entity)};checkDamage(entity) > 0)
            entity->GetLogicComponent()->QueueUpEvent(Event_Damage::Create(damage), true);
    }
    lastDamagePoints = damagePoints;
    damagePoints.clear();
    if (SpawnSlimes && entities.size() < MAXENTITYCOUNT) tryToSpawnSlime();
}


IEntity* Server::GetPlayer() {
    std::shared_lock lock(serverMutex);
    if (localPlayer) return localPlayer.get();
    return nullptr; // Return nullptr if entity not found
}

IEntity * Server::GetPlayer_unprotected() const {
    if (localPlayer) return localPlayer.get();
    return nullptr; // Return nullptr if entity not found
}

std::shared_ptr<Server> Server::GetSharedPtr() {
    return shared_from_this();
}

std::vector<DamageArea> Server::GetDamagePoints() {
    std::shared_lock lock(serverMutex);
    return lastDamagePoints;
}

AiManager & Server::GetAiManager() {
    std::shared_lock lock(serverMutex);
    return aiManager;
}

AiManager& Server::GetAiManager_unprotected() {
    return aiManager;
}

WorldData & Server::GetWorldData() {
    return worldData;
}

std::vector<Coordinates> Server::GetRespawnPoints() {
    return respawnPoints;
}

std::map<int,std::shared_ptr<IEntity>> Server::GetEntities() {
    std::shared_lock lock(serverMutex);
    return entities;
}

std::map<int,std::shared_ptr<IStructure>> Server::GetStructures() {
    std::shared_lock lock(serverMutex);
    return structures;
}

Coordinates Server::GetEntityPos(int entityId) {
   std::shared_lock lock(serverMutex);
    if (entities.contains(entityId)) {
        return entities[entityId]->GetCoordinates();
    }
    return Coordinates{0.0f, 0.0f}; // Return a default value if entity not found
}

IEntity* Server::GetEntity(const int entityId) {
    std::shared_lock lock(serverMutex);
    if (entities.contains(entityId)) {
        return entities[entityId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

IEntity * Server::GetEntity_unprotected(const int entityId) const {
    if (entities.contains(entityId)) {
        return entities.at(entityId).get();
    }
    return nullptr;
}

IStructure* Server::GetStructure(int structureId) {
    std::shared_lock lock(serverMutex);
    if (structures.contains(structureId)) {
        return structures[structureId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

void Server::GenerateStructures() {
    struct biomeModifierInfo {
        int biomeId;
        double treeDensityModifier;
        double oreDensityModifier;
        Tree::TreeVariant treeVariant{Tree::TreeVariant::NONE};
        double oreRarityModifier{0};
    };

    const std::vector<biomeModifierInfo> biomeModifierValues = {
        {1, 0.0,0.1, Tree::TreeVariant::NONE, 0},//Beach
        {2, 0.0,0.7,Tree::TreeVariant::NONE, 0.1}, //Desert
        {3,0.5,0.5, Tree::TreeVariant::PLAINS, 0.05}, //Grass
        {4, 0.0,1.5, Tree::TreeVariant::NONE, 0.2},//Mountain
        {5,1.3,0.3,Tree::TreeVariant::FOREST,0.05}, //Forest
        {6,0.3,1.3, Tree::TreeVariant::SNOW,0.15}  //Snow
    };

    constexpr int commonOreVariants{2}; //Pocet variant rud, IRON, COPPER
    constexpr int rareOreVariants{3}; //Pocet variant IRON, COPPER, GOLD

    std::mt19937 mt(seed );
    std::uniform_int_distribution dist(1,100);
    std::uniform_int_distribution<> oreDist(1,2); //Pro ruzne varianty rud

    auto TryToSpawnTree = [&](const biomeModifierInfo &biome, const Coordinates pos)-> bool {
        if (dist(mt) > TREEDENSITY* biome.treeDensityModifier) return false;
        return AddStructure_unprotected(pos, structureType::TREE, static_cast<int>(biome.treeVariant));
    };

    auto TryToSpawnOre = [&](const biomeModifierInfo &biome, const Coordinates pos)-> bool {
        if (dist(mt) > OREDENSITY* biome.oreDensityModifier) return false;

        const int oreAmount = (dist(mt) < biome.oreRarityModifier * 100) ? rareOreVariants : commonOreVariants;
        std::uniform_int_distribution<> oreTypeDist(1,oreAmount);

        if (dist(mt) < OREDEPOSIT*10) return AddStructure_unprotected(pos, structureType::ORE_DEPOSIT, oreTypeDist(mt), oreDist(mt));
        return AddStructure_unprotected(pos, structureType::ORE_NODE, oreTypeDist(mt), oreDist(mt));
    };

    auto checkForDiagonalNeighbors = [&](const int x, const int y) -> int {
        return
      GetMapValue_unprotected(x-1, y-1, WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x+1, y-1, WorldData::COLLISION_MAP);
    };

    auto checkForNeighbors = [&](const int x, const int y) -> int {
        return
      GetMapValue_unprotected(x-1, y-1, WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x,   y-1, WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x+1, y-1, WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x-1, y,   WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x+1, y,   WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x-1, y+1, WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x,   y+1, WorldData::COLLISION_MAP) +
      GetMapValue_unprotected(x+1, y+1, WorldData::COLLISION_MAP);
    };

    auto process = [&](const int x, const int y) {
        if (checkForDiagonalNeighbors(x,y) > 0) return;
        if (checkForNeighbors(x,y) > 2) return;

        if (GetMapValue_unprotected(x-1, y-1, WorldData::COLLISION_MAP)
            + GetMapValue_unprotected(x+1, y-1, WorldData::COLLISION_MAP) > 0) return;
        auto Biome = biomeModifierValues.at(0);
        const int biomeValue = GetMapValue_unprotected(x, y, WorldData::BIOME_MAP);
        for (const auto& biomeInfo : biomeModifierValues) {
            if (biomeInfo.biomeId != biomeValue) continue;
            Biome = biomeInfo;
            break;
        }
        if (TryToSpawnTree(Biome,toWorldCoordinates(x,y))){}
        else TryToSpawnOre(Biome,toWorldCoordinates(x,y));
    };


    std::lock_guard lock(serverMutex);
    for (int x = 0; x < MAPSIZE; x++) {
        for (int y = 0; y < MAPSIZE; y++) process(x, y);
    }
}

void Server::GenerateWorld(){
    std::lock_guard lock(serverMutex);
    GeneraceMapy generaceMapy(seed);

    std::mt19937 mt(seed);
    std::uniform_real_distribution<double> dist(1.0,VARIATION_LEVELS);

    //TODO: implementovat přímo do generace generace mapy
    for (int x = 0; x < generaceMapy.biomMapa.size(); x++) {
        for (int y = 0; y < generaceMapy.biomMapa.at(x).size(); y++) {
            int biomeValue = generaceMapy.biomMapa.at(x).at(y);
            //přesun dat do matice
            worldData.UpdateMapValue(x,y,WorldData::BIOME_MAP,biomeValue);

            //Random block variation level
            int variation = static_cast<int>(dist(mt));
            worldData.UpdateMapValue(x,y,WorldData::BLOCK_VARIATION_MAP,variation);

            if (biomeValue == 0) worldData.UpdateMapValue(x,y,WorldData::COLLISION_MAP, -1); // Water
            else worldData.UpdateMapValue(x,y,WorldData::COLLISION_MAP,0);
        }
    }
}

void Server::SetItemDropCallback(std::function<bool(std::unique_ptr<Item>)> callback) {
    onItemDropped = std::move(callback);
}

bool Server::AddItemToInventory(std::unique_ptr<Item> item) const {
    if (onItemDropped && item) {
        return onItemDropped(std::move(item));
    }
    return false;
}

void Server::SetInventorySaveCallback(std::function<void(const std::string&)> callback) {
    onSaveInventory = std::move(callback);
}

void Server::SetInventoryLoadCallback(std::function<void(const std::string&)> callback) {
    onLoadInventory = std::move(callback);
}

