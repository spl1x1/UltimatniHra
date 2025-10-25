//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef SERVERSTRUCS_H
#define SERVERSTRUCS_H


struct Server{
    int maxPlayers;
    int port;
    const char* ipAddress;

    float deltaTime;
};

#endif //SERVERSTRUCS_H
