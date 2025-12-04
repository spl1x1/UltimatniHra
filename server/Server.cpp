//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#include "Server.h"
#include "../MACROS.h"
#include "World/generace_mapy.h"
#include "Entities/Entity.h"

Coordinates Server::getPlayerEntityCoords() {
    serverMutex.lock();
    //List by měl obsahovat minimálně jednoho hráče
    const Coordinates coordinates =entities.at(0).coordinates;

    serverMutex.unlock();
    return coordinates;
;
}


void Server::updatePlayerEntityCopy(const Entity *playerEntity) {
    serverMutex.lock();

    if (entities.empty()) entities.push_back(*playerEntity);
    else entities.at(0)= *playerEntity;

    serverMutex.unlock();
}

void Server::generateWorld() const {
    auto *generaceMapy = new GeneraceMapy(8);

    std::srand(static_cast<unsigned int>(seed));
    std::mt19937 mt(seed);
    std::uniform_real_distribution<double> dist(1.0,VARIATION_LEVELS);

    //TODO: implementovat přímo do generace generace mapy
    for (int x = 0; x < generaceMapy->biomMapa.size(); x++) {
        for (int y = 0; y < generaceMapy->biomMapa.at(x).size(); y++) {

            //přesun dat do matice
            worldData.biomeMap[x][y] = generaceMapy->biomMapa.at(x).at(y);

            //Random block variation level
            int variation = static_cast<int>(dist(mt));
            worldData.blockVariantionMap[x][y] = variation;

            //Generate structure map
            switch (worldData.biomeMap[x][y]) {
                case 0: {
                    worldData.collisionMap[x][y] = 1; // Water, Player cant enter
                    break;
                }
                default: {
                    worldData.collisionMap[x][y] = 0; // Empty, Player can enter
                    break;
                }
            }
        }
    }
    delete generaceMapy;
}
