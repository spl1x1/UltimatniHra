//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#include "../../include/Server/Server.h"

#include <ranges>
#include <shared_mutex>

#include "../../include/MACROS.h"
#include "../../include/Server/generace_mapy.h"
#include "../../include/Entities/Entity.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Sprites/Sprite.hpp"
#include "../../include/Structures/Structure.h"
#include "../../include/Structures/Tree.h"


void Server::setEntityPos(int entityId, Coordinates newCoordinates) {
    std::lock_guard lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        _entities[entityId]->coordinates = newCoordinates;
    }
}

void Server::setPlayerPos(int playerId, Coordinates newCoordinates) {
    std::lock_guard lock(serverMutex);
    if (_players.find(playerId) != _players.end()) {
        _players[playerId]->coordinates = newCoordinates;
    }
}

void Server::setEntityCollision(int entityId, bool disable) {
    std::lock_guard lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        _entities[entityId]->disableCollision(disable);
    }
}

void Server::setPlayerCollision(int playerId, bool disable) {
    std::lock_guard lock(serverMutex);
    if (_players.find(playerId) != _players.end()) {
        _players[playerId]->disableCollision(disable);
    }
}

bool Server::isEntityColliding(int entityId) {
    std::shared_lock lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        return _entities[entityId]->isColliding();
    }
    return false; // Return false if entity not found
}

bool Server::isPlayerColliding(int playerId) {
    std::shared_lock lock(serverMutex);
    if (_players.find(playerId) != _players.end()) {
        return _players[playerId]->isColliding();
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

int Server::getMapValue(int x, int y, WorldData::MapType mapType) {
    std::shared_lock lock(serverMutex);
    return _worldData.getMapValue(x,y, mapType);
}

void Server::setMapValue(int x, int y, WorldData::MapType mapType, int value) {
    std::lock_guard lock(serverMutex);
    _worldData.updateMapValue(x,y, mapType, value);
}

void Server::setMapValue_unprotected(int x, int y, WorldData::MapType mapType, int value) {
    _worldData.updateMapValue(x,y, mapType, value);
}

void Server::addPlayer(const std::shared_ptr<Entity>& player) {
    std::lock_guard lock(serverMutex);
    int newId = getNextPlayerId();
    player->id = newId;
    _players[newId] = player;
}

void Server::addEntity(const std::shared_ptr<Entity>& entity) {
    std::lock_guard lock(serverMutex);
    int newId = getNextEntityId();
    entity->id = newId;
    _entities[newId] = entity;
}

void Server::addStructure(Coordinates coordinates, structureType type) {
    std::lock_guard lock(serverMutex);
    int newId = getNextStructureId();

    switch (type) {
        case structureType::TREE: {;
            auto newTree = std::make_shared<Tree>(newId, coordinates, getSharedPtr());
            _structures[newId] = newTree;
            break;
        }
        default:
            break;
    }
}



void Server::playerUpdate(PlayerEvent e) {
    std::lock_guard lock(serverMutex);
    e.deltaTime = _deltaTime;
    Player* player = dynamic_cast<Player*>(_players[0].get());
    if (player) {
        player->handleEvent(e);
    }
}

void Server::Tick() {
    std::lock_guard lock(serverMutex);
    for (auto &entity: _entities | std::views::values) {
        if (entity == nullptr || entity->sprite == nullptr) continue;
        entity->sprite->tick(_deltaTime);
    }
    for (auto &player: _players | std::views::values) {
        if (player == nullptr || player->sprite == nullptr) continue;
        player->sprite->tick(_deltaTime);
    }
}

std::map<int,std::shared_ptr<class Entity>> Server::getPlayers() {
    std::shared_lock lock(serverMutex);
    return _players;
}

Coordinates Server::getPlayerPos(int playerId) {
    std::shared_lock lock(serverMutex);
    if (_players.find(playerId) != _players.end()) {
        return _players[playerId]->coordinates;
    }
    return Coordinates{0.0f, 0.0f}; // Return a default value if entity not found
}

Entity* Server::getPlayer(int playerId) {
    std::shared_lock lock(serverMutex);
    if (_players.find(playerId) != _players.end()) {
        return _players[playerId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

std::shared_ptr<Server> Server::getSharedPtr() {
    return shared_from_this();
}

std::map<int,std::shared_ptr<class Entity>> Server::getEntities() {
    std::shared_lock lock(serverMutex);
    return _entities;
}

std::map<int,std::shared_ptr<IStructure>> Server::getStructures() {
    std::shared_lock lock(serverMutex);
    return _structures;
}

Coordinates Server::getEntityPos(int entityId) {
   std::shared_lock lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        return _entities[entityId]->coordinates;
    }
    return Coordinates{0.0f, 0.0f}; // Return a default value if entity not found
}

Entity* Server::getEntity(int entityId) {
    std::shared_lock lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        return _entities[entityId].get();
    }
    return nullptr; // Return nullptr if entity not found
}

IStructure* Server::getStructure(int structureId) {
    std::shared_lock lock(serverMutex);
    if (_structures.find(structureId) != _structures.end()) {
        return _structures[structureId].get();
    }
    return nullptr; // Return nullptr if entity not found
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