//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#include "../../include/Server/Server.h"

#include <ranges>
#include <shared_mutex>

#include "../../include/Application/MACROS.h"
#include "../../include/Server/generace_mapy.h"
#include "../../include/Entities/Entity.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Entities/Slime.h"
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
        [](IEntity* entity, float dt) {
            SDL_Log("Slime %d: Idling", entity->GetId());
        },
        [](IEntity* entity, float dt) {}
    );

    sm->registerState(AiState::Patrol,
        [](IEntity* entity, float dt) {
            SDL_Log("Slime %d: Patrolling", entity->GetId());
            std::mt19937 mt(static_cast<unsigned int>(SDL_GetTicks()));
            std::uniform_int_distribution dist(1,entity->GetReach());
            entity->GetLogicComponent()->AddEvent(Event_MoveTo::Create(
            entity->GetCoordinates().x + dist(mt),
            entity->GetCoordinates().y + dist(mt)
        ));
    },
    [](IEntity* entity, float dt) {});

    sm->registerState(AiState::Chase,
        // onEnter - zavolá se jednou při vstupu do stavu
        [](IEntity* entity, float dt) {
            SDL_Log("Slime %d: Entering Chase state", entity->GetId());
        },
        // onUpdate - zavolá se každý frame (prázdný pro Chase)
        [](IEntity* entity, float dt) {
            if (const auto localPlayer{entity->GetServer()->localPlayer}) {
            const auto dx{localPlayer->GetEntityCenter().x - entity->GetEntityCenter().x};
            const auto dy {localPlayer->GetEntityCenter().y - entity->GetEntityCenter().y};
            entity->GetLogicComponent()->AddEvent(Event_Move::Create(dx,dy));
}
        }
    );


    sm->registerState(AiState::Attack,
        [](IEntity* entity, float dt) {
            SDL_Log("Slime %d: Attacking player", entity->GetId());
        },
        [](IEntity* entity, float dt) {
            entity->GetLogicComponent()->SetAngle(
            CalculateAngle(entity->GetEntityCenter(), entity->GetServer()->GetPlayer_unprotected()->GetEntityCenter()));
            entity->GetLogicComponent()->PerformAttack(entity, 0, 10);
        }
    );

    sm->registerState(AiState::GetUnstuck,
        [](IEntity* entity, float dt) {
            SDL_Log("Slime %d: Attempting to get unstuck", entity->GetId());
            const auto [x, y]{entity->GetServer()->GetPlayer_unprotected()->GetEntityCenter()};
            entity->GetLogicComponent()->AddEvent(Event_MoveTo::Create(x,y));
        },
        [](IEntity* entity, float dt) {
            // Po pokusu o odblokování se vrať zpět do Chase stavu
            const auto logicComp{entity->GetLogicComponent()};
            const auto distance = CoordinatesDistance(logicComp->GetCoordinates(),logicComp->GetLastMoveDirection());
            if (std::abs(distance) <  EntityLogicComponent::threshold) {
                entity->GetServer()->aiManager.sendEvent(entity, AiEvent::ReachedDestination);
                entity->GetLogicComponent()->AddEvent(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO),true);
            }
        }
    );

    // Registrace přechodů
    sm->registerTransition(AiState::Idle, AiEvent::PlayerSpotted, AiState::Chase);
    sm->registerTransition(AiState::Chase, AiEvent::TargetInRange, AiState::Attack);
    sm->registerTransition(AiState::Chase, AiEvent::PlayerLost, AiState::Idle);
    sm->registerTransition(AiState::Attack, AiEvent::TargetOutOfRange, AiState::Chase);

    sm->registerTransition(AiState::Idle, AiEvent::TookDamage, AiState::Chase);
    sm->registerTransition(AiState::Patrol, AiEvent::TookDamage, AiState::Chase);

    sm->registerTransition(AiState::Chase, AiEvent::MovementStuck, AiState::GetUnstuck);
    sm->registerTransition(AiState::GetUnstuck, AiEvent::ReachedDestination, AiState::Chase);
    sm->registerTransition(AiState::GetUnstuck, AiEvent::TargetInRange, AiState::Attack);

    sm->registerTransition(AiState::Patrol, AiEvent::ReachedDestination, AiState::Patrol);
}


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
    return worldData.getMapValue(x,y, mapType);
}

int Server::GetMapValue_unprotected(int x, int y, WorldData::MapType mapType) const {
    if (x < 0 || y < 0 || x >= MAPSIZE || y >= MAPSIZE) {
        return -1; // Out of bounds
    }
    return worldData.getMapValue(x,y, mapType);
}


void Server::SetMapValue(int x, int y, WorldData::MapType mapType, int value) {
    std::lock_guard lock(serverMutex);
    worldData.updateMapValue(x,y, mapType, value);
}

void Server::SetMapValue_unprotected(int x, int y, WorldData::MapType mapType, int value) const {
    worldData.updateMapValue(x,y, mapType, value);
}

void Server::AddLocalPlayer(const std::shared_ptr<Player>& player) {
    std::lock_guard lock(serverMutex);
    localPlayer = player;
    entities[0] = player;
    player->SetId(0); //Local player always has ID 0
}

void Server::AddEntity_unprotected(const std::shared_ptr<IEntity>& entity) {
    const int newId = getNextEntityId();
    entities.insert_or_assign(newId, entity);
    entity->SetId(newId);
}

void Server::AddEntity(const std::shared_ptr<IEntity>& entity) {
    std::lock_guard lock(serverMutex);
    const int newId = getNextEntityId();
    entities.insert_or_assign(newId, entity);
    entity->SetId(newId);
}


void Server::AddStructure(Coordinates coordinates, structureType type, int innerType, int variant) {
    std::lock_guard lock(serverMutex);
    int newId = getNextStructureId();

    std::shared_ptr<IStructure> newStructure;
    switch (type) {
        case structureType::TREE: {
            newStructure = std::make_shared<Tree>(newId, coordinates, GetSharedPtr(), static_cast<Tree::TreeVariant>(innerType));
            break;
        }
        case structureType::ORE_NODE: {
            newStructure = std::make_shared<OreNode>(newId, coordinates, GetSharedPtr(), static_cast<OreType>(innerType), variant);
            break;
        }
            case structureType::ORE_DEPOSIT: {
            newStructure = std::make_shared<OreDeposit>(newId, coordinates, GetSharedPtr(), static_cast<OreType>(innerType), variant);
        }
        default:
            break;
    }

    if (!newStructure->wasProperlyInitialized()) {
        SDL_Log("Failed to create Tree structure at (%f, %f)", coordinates.x, coordinates.y);
        nextStructureId--; //Rollback ID pokud se nepovede vytvorit struktura
        return;
    }
    structures[newId] =newStructure;
}

bool Server::AddStructure_unprotected(Coordinates coordinates, structureType type, int innerType, int variant) {
    int newId = getNextStructureId();

    std::shared_ptr<IStructure> newStructure;
    switch (type) {
        case structureType::TREE: {
            newStructure = std::make_shared<Tree>(newId, coordinates, GetSharedPtr(), static_cast<Tree::TreeVariant>(innerType));
            break;
        }
        case structureType::ORE_NODE: {
            newStructure = std::make_shared<OreNode>(newId, coordinates, GetSharedPtr(), static_cast<OreType>(innerType), variant);
            break;
        }
        case structureType::ORE_DEPOSIT: {
            newStructure = std::make_shared<OreDeposit>(newId, coordinates, GetSharedPtr(), static_cast<OreType>(innerType), variant);
        }
        default:
            break;
    }

    if (!newStructure->wasProperlyInitialized()) {
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
    const auto mapValue{worldData.getMapValue(tileX, tileY, WorldData::COLLISION_MAP)};

     std::vector<std::string> text;

    for (const auto &entity: entities | std::views::values) {
        const auto entityPos{entity->GetEntityCenter()};
        const int entityTileX{static_cast<int>(std::floor(entityPos.x / 32.0f))};
        const int entityTileY{static_cast<int>(std::floor(entityPos.y / 32.0f))};
        if (entityTileX == tileX && entityTileY == tileY) {
            text.emplace_back(EntityRenderingComponent::TypeToString(entity->GetType()));
        }
    }

    if ( mapValue> 0) text.emplace_back(StructureRenderingComponent::TypeToString(GetStructure(mapValue)->getType()));

    return text;
}

void Server::AddEntity(Coordinates coordinates, const EntityType type, const int variant)
{
    std::unique_lock lock(serverMutex);
    switch (type) {
        case EntityType::SLIME: {
            const auto slime{std::make_shared<Slime>(GetSharedPtr().get(), coordinates)};
            AddEntity_unprotected(slime);
            if (slime) setupSlimeAi(slime.get());
            break;
        }
        case EntityType::PLAYER: {
            const auto player{std::make_shared<Player>(GetSharedPtr().get(), coordinates)};
            AddEntity_unprotected(player);
            break;
        }
        default:
            break;
    }
}

void Server::Tick() {
    auto playerDetection = [this](IEntity* player) {
        for (auto &entity: entities | std::views::values) {
            if (entity->GetType() == EntityType::SLIME) {
                if (const auto distanceToPlayer {CoordinatesDistance(entity->GetEntityCenter(), player->GetEntityCenter())}; distanceToPlayer < entity->GetDetectionRange()) {
                    aiManager.sendEvent(entity.get(), AiEvent::PlayerSpotted);
                    if (distanceToPlayer < entity->GetAttackRange()) aiManager.sendEvent(entity.get(), AiEvent::TargetInRange);
                    else aiManager.sendEvent(entity.get(), AiEvent::TargetOutOfRange);
                } else {
                    aiManager.sendEvent(entity.get(), AiEvent::PlayerLost);
                }
            }
        }
    };
    auto checkDamage = [this](const std::shared_ptr<IEntity>& entity) ->int {
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
        entities.erase(entityId);
        reclaimedEntityIds.emplace_back(entityId);
    }
    entitiesToRemove.clear();

    playerDetection(localPlayer.get()); //Detekce hrace pro AI, zatim pouze lokalni hrac
    aiManager.update(deltaTime);

    for (const auto& structureId : structuresToRemove) {
        structures.erase(structureId);
        reclaimedStructureIds.emplace_back(structureId);
    }
    structuresToRemove.clear();

    for (const auto &entity: entities | std::views::values) {
        if (!entity) continue;
        const auto damage =checkDamage(entity);
        entity->AddEvent(Event_Damage::Create(damage));
        entity->Tick();
    }
    lastDamagePoints = damagePoints;
    damagePoints.clear();
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
    auto *generaceMapy = new GeneraceMapy(8);

    std::mt19937 mt(seed);
    std::uniform_real_distribution<double> dist(1.0,VARIATION_LEVELS);

    //TODO: implementovat přímo do generace generace mapy
    for (int x = 0; x < generaceMapy->biomMapa.size(); x++) {
        for (int y = 0; y < generaceMapy->biomMapa.at(x).size(); y++) {
            int biomeValue = generaceMapy->biomMapa.at(x).at(y);
            //přesun dat do matice
            worldData.updateMapValue(x,y,WorldData::BIOME_MAP,biomeValue);

            //Random block variation level
            int variation = static_cast<int>(dist(mt));
            worldData.updateMapValue(x,y,WorldData::BLOCK_VARIATION_MAP,variation);

            if (biomeValue == 0) worldData.updateMapValue(x,y,WorldData::COLLISION_MAP, -1); // Water
            else worldData.updateMapValue(x,y,WorldData::COLLISION_MAP,0);
        }
    }
    delete generaceMapy;
}