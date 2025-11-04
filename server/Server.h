//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef SERVERSTRUCS_H
#define SERVERSTRUCS_H
#include "World/WorldStructs.h"


class Server{
public:
    int maxPlayers;
    int port;
    int seed;
    const char* ipAddress;
    WorldData worldData;

    float deltaTime;

    void generateWorld();
};

#endif //SERVERSTRUCS_H
