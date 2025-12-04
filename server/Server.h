//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef SERVERSTRUCS_H
#define SERVERSTRUCS_H
#include <mutex>
#include <vector>

#include "World/WorldStructs.h"

//Defines a 2D coordinate
struct Coordinates {
    float x = 0;
    float y = 0;
};

class Server {
    std::mutex serverMutex;
public:
    int port;
    int seed;
    const char* ipAddress;
    WorldData worldData;
    Coordinates spawnPoint = {4000.0f, 4000.0f};
    std::vector<class Entity> entities{};

    float deltaTime;
    void generateWorld() const;
    void updatePlayerEntityCopy(const Entity *playerEntity);
    Coordinates getPlayerEntityCoords();
};

#endif //SERVERSTRUCS_H
