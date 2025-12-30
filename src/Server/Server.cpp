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
#include "../../include/Structures/Structure.h"
#include "../../include/Structures/Tree.h"
#include "../../include/Window/WorldStructs.h"



void Server::setEntityPos(int entityId, Coordinates newCoordinates) {
    std::lock_guard lock(serverMutex);
    if (_entities.contains(entityId)) {
        _entities[entityId]->GetCoordinates() = newCoordinates;
    }
}

void Server::setPlayerPos(int playerId, Coordinates newCoordinates) {
    std::lock_guard lock(serverMutex);
    if (_players.contains(playerId)) {
        _players[playerId]->GetCoordinates() = newCoordinates;
    }
}

void Server::setEntityCollision(int entityId, bool disable) {
    std::lock_guard lock(serverMutex);
    if (_entities.contains(entityId)) {
        _entities[entityId]->SetEntityCollision(disable);
    }
}

void Server::setPlayerCollision(int playerId, bool disable) {
    std::lock_guard lock(serverMutex);
    if (_players.contains(playerId)) {
        _players[playerId]->SetEntityCollision(disable);
    }
}

bool Server::isEntityColliding(int entityId) {
    std::shared_lock lock(serverMutex);
    if (_entities.contains(entityId)) {
        return _entities[entityId]->GetCollisionStatus().colliding;
    }
    return false; // Return false if entity not found
}

bool Server::isPlayerColliding(int playerId) {
    std::shared_lock lock(serverMutex);
    if (_players.contains(playerId)) {
        return _players[playerId]->GetCollisionStatus().colliding == true;
    }
    return false; // Return false if entity not found
}

void Server::setDeltaTime(float dt) {
    std::lock_guard lock(serverMutex);
    _deltaTime = dt;
}

float Server::getDeltaTime(){
    std::shared_lock lock(serverMutex);
    return _deltaTime;
}
float Server::getDeltaTime_unprotected() const{
    return _deltaTime;
}

int Server::getMapValue(int x, int y, WorldData::MapType mapType) {
    std::shared_lock lock(serverMutex);
    return _worldData.getMapValue(x,y, mapType);
}

void Server::setMapValue(int x, int y, WorldData::MapType mapType, int value) {
    std::lock_guard lock(serverMutex);
    _worldData.updateMapValue(x,y, mapType, value);
}

void Server::setMapValue_unprotected(int x, int y, WorldData::MapType mapType, int value) const {
    _worldData.updateMapValue(x,y, mapType, value);
}

void Server::addPlayer(const std::shared_ptr<IEntity>& player) {
    std::lock_guard lock(serverMutex);
    int newId = getNextPlayerId();
    _players[newId] = player;
}

void Server::addEntity(const std::shared_ptr<IEntity>& entity) {
    std::lock_guard lock(serverMutex);
    int newId = getNextEntityId();
    _entities[newId] = entity;
}


void Server::addStructure(Coordinates coordinates, structureType type) {
    std::lock_guard lock(serverMutex);
    int newId = getNextStructureId();

    std::shared_ptr<IStructure> newStructure;
    switch (type) {
        case structureType::TREE: {
            newStructure = std::make_shared<Tree>(newId, coordinates, getSharedPtr());
            break;
        }
        default:
            break;
    }

    if (!newStructure->wasProperlyInitialized()) {
        SDL_Log("Failed to create Tree structure at (%f, %f)", coordinates.x, coordinates.y);
        _nextStructureId--; //Rollback ID pokud se nepovede vytvorit struktura
        return;
    }
    _structures[newId] =newStructure;
}

void Server::addStructure_unprotected(Coordinates coordinates, structureType type) {
    int newId = getNextStructureId();

    std::shared_ptr<IStructure> newStructure;
    switch (type) {
        case structureType::TREE: {
            newStructure = std::make_shared<Tree>(newId, coordinates, getSharedPtr());
            break;
        }
        default:
            break;
    }

    if (!newStructure->wasProperlyInitialized()) {
        SDL_Log("Failed to create Tree structure at (%f, %f)", coordinates.x, coordinates.y);
        _nextStructureId--; //Rollback ID pokud se nepovede vytvorit struktura
        return;
    }
    _structures[newId] =newStructure;

}

void Server::playerUpdate(EventData e) {
    std::lock_guard lock(serverMutex);
    e.dt = _deltaTime;
    if (const auto playerEntity = dynamic_cast<Player*>(_players[0].get())) {
        playerEntity->AddEvent(e);
    }
}

std::set<int> Server::getStructuresInArea(Coordinates topLeft, Coordinates bottomRight) {
    std::lock_guard lock(serverMutex);
    if (std::abs(topLeft.x - cacheValidityData.lastPlayerPos.x) >= static_cast<float>(cacheValidityData.rangeForCacheUpdate)
        ||std::abs(topLeft.y - cacheValidityData.lastPlayerPos.y) >= static_cast<float>(cacheValidityData.rangeForCacheUpdate))
        cacheValidityData.isCacheValid = false;


    if (cacheValidityData.isCacheValid) {
        return EntityIdCache;
    }

    SDL_Log("Refreshing structure ID cache");
    cacheValidityData.lastPlayerPos = topLeft;
    EntityIdCache.clear();
    const int rangeXMin = static_cast<int>(std::floor((topLeft.x) / 32.0f)) - cacheValidityData.rangeForCacheUpdate/32;
    const int rangeYMin = static_cast<int>(std::floor((topLeft.y) / 32.0f)) - cacheValidityData.rangeForCacheUpdate/32;
    const int rangeXMax = static_cast<int>(std::ceil((bottomRight.x) / 32.0f)) + cacheValidityData.rangeForCacheUpdate/32;
    const int rangeYMax = static_cast<int>(std::ceil((bottomRight.y) / 32.0f)) + cacheValidityData.rangeForCacheUpdate/32;

    for (int x = rangeXMin; x <= rangeXMax; x++) {
        if (x < 0 || x >= MAPSIZE) continue;
        for (int y = rangeYMin; y <= rangeYMax; y++) {
            if (y < 0 || y >= MAPSIZE) continue;
            if (int id = getMapValue_unprotected(x, y, WorldData::COLLISION_MAP); id > 0)  EntityIdCache.emplace(id);
        }
    }
    cacheValidityData.isCacheValid = true;
    return EntityIdCache;
}

void Server::Tick() {
    std::lock_guard lock(serverMutex);
    for (auto &entity: _entities | std::views::values) {
        if (!entity) continue;
        entity->Tick();
    }
    for (auto &player: _players | std::views::values) {
        if (!player) continue;
        player->Tick();
    }
}

std::map<int,std::shared_ptr<IEntity>> Server::getPlayers() {
    std::shared_lock lock(serverMutex);
    return _players;
}

Coordinates Server::getPlayerPos(int playerId) {
    std::shared_lock lock(serverMutex);
    if (_players.contains(playerId)) {
        return _players[playerId]->GetCoordinates();
    }
    return Coordinates{0.0f, 0.0f}; // Return a default value if entity not found
}

IEntity* Server::getPlayer(int playerId) {
    std::shared_lock lock(serverMutex);
    if (_players.contains(playerId)) {
        return _players[playerId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

std::shared_ptr<Server> Server::getSharedPtr() {
    return shared_from_this();
}

std::map<int,std::shared_ptr<IEntity>> Server::getEntities() {
    std::shared_lock lock(serverMutex);
    return _entities;
}

std::map<int,std::shared_ptr<IStructure>> Server::getStructures() {
    std::shared_lock lock(serverMutex);
    return _structures;
}

Coordinates Server::getEntityPos(int entityId) {
   std::shared_lock lock(serverMutex);
    if (_entities.contains(entityId)) {
        return _entities[entityId]->GetCoordinates();
    }
    return Coordinates{0.0f, 0.0f}; // Return a default value if entity not found
}

IEntity* Server::getEntity(int entityId) {
    std::shared_lock lock(serverMutex);
    if (_entities.contains(entityId)) {
        return _entities[entityId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

IStructure* Server::getStructure(int structureId) {
    std::shared_lock lock(serverMutex);
    if (_structures.contains(structureId)) {
        return _structures[structureId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

void Server::generateTrees(){
    struct biomeTreeInfo {
        int biomeId;
        double densityModifier;
    };

    std::vector<biomeTreeInfo> biomesWithTrees = {
        {3,0.5}, //Grass
        {5,1.3}, //Forest
        {6,0.3}  //Snow
    };

    std::mt19937 mt(_seed );
    std::uniform_int_distribution dist(1,100);

    std::lock_guard lock(serverMutex);
    for (int x = 0; x < MAPSIZE; x++) {
        for (int y = 0; y < MAPSIZE; y++) {

            double biomeModifier = 0.0;
            int biomeValue = getMapValue_unprotected(x, y, WorldData::BIOME_MAP);
            for (const auto& biomeInfo : biomesWithTrees) {
                if (biomeInfo.biomeId == biomeValue) {
                    biomeModifier = biomeInfo.densityModifier;
                    break;
                }
            }
            if (biomeModifier == 0.0) continue; //Pokud neni v biomu povoleno generovat stromy, pokracuj dale

            int roll = dist(mt);
            if (roll > TREEDENSITY* biomeModifier) continue;
            Coordinates position = {static_cast<float>(x*32),static_cast<float>(y*32)};
            addStructure_unprotected(position, structureType::TREE);
        }
    }
}

void Server::generateWorld(){
    std::lock_guard lock(serverMutex);
    auto *generaceMapy = new GeneraceMapy(8);

    std::srand(static_cast<unsigned int>(_seed));
    std::mt19937 mt(_seed);
    std::uniform_real_distribution<double> dist(1.0,VARIATION_LEVELS);

    //TODO: implementovat přímo do generace generace mapy
    for (int x = 0; x < generaceMapy->biomMapa.size(); x++) {
        for (int y = 0; y < generaceMapy->biomMapa.at(x).size(); y++) {
            int biomeValue = generaceMapy->biomMapa.at(x).at(y);
            //přesun dat do matice
            _worldData.updateMapValue(x,y,WorldData::BIOME_MAP,biomeValue);

            //Random block variation level
            int variation = static_cast<int>(dist(mt));
            _worldData.updateMapValue(x,y,WorldData::BLOCK_VARIATION_MAP,variation);

            if (biomeValue == 0) _worldData.updateMapValue(x,y,WorldData::COLLISION_MAP,1);
            else _worldData.updateMapValue(x,y,WorldData::COLLISION_MAP,0);
        }
    }
    delete generaceMapy;
}