//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#include "Server.h"

#include <shared_mutex>

#include "../MACROS.h"
#include "World/generace_mapy.h"
#include "Entities/Entity.h"
#include "Entities/Player.hpp"

void Server::setEntityPos(int entityId, Coordinates newCoordinates) {
    std::lock_guard lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        _entities[entityId]->coordinates = newCoordinates;
    }
}

void Server::setEntityCollision(int entityId, bool disable) {
    std::lock_guard lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        _entities[entityId]->disableCollision(disable);
    }
}
bool Server::isEntityColliding(int entityId) {
    std::shared_lock lock(serverMutex);
    if (_entities.find(entityId) != _entities.end()) {
        return _entities[entityId]->isColliding();
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

int Server::getCollisionMapValue(int x, int y, WorldData::MapType mapType) {
    std::shared_lock lock(serverMutex);
    return _worldData.getMapValue(x,y, mapType);
}

void Server::addEntity(Entity *entity) {
    std::lock_guard lock(serverMutex);
    int newId = getNextEntityId();
    entity->id = newId;
    _entities[newId] = entity;
}

int Server::getNextEntityId() {
    std::lock_guard lock(serverMutex);
    return _nextEntityId++;
}

void Server::playerUpdate(const PlayerEvent e) {
    std::lock_guard lock(serverMutex);
    reinterpret_cast<Player*>(_entities[0])->handleEvent(e);
}

void Server::Tick() {
    std::lock_guard lock(serverMutex);
    for (auto& [id, entity] : _entities) {
        if (entity == nullptr) continue;
        entity->sprite->tick(_deltaTime);
    }
}

std::list<Sprite> Server::getEntitySprites() {
    std::shared_lock lock(serverMutex);
    std::list<Sprite> entityList;
    for (auto pair: _entities) {
        if (pair.second == nullptr) continue;
        entityList.push_back(*pair.second->sprite);
    }
    return entityList;
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
        return _entities[entityId];
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