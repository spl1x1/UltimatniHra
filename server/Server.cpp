//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#include "Server.h"
#include "../MACROS.h"
#include "World/generace_mapy.h"

void Server::generateWorld() {
    auto *generaceMapy = new GeneraceMapy(seed);

    // std::srand(static_cast<unsigned int>(seed));
    // std::mt19937 mt(seed);
    // std::uniform_real_distribution<double> dist(1.0,VARIATION_LEVELS);

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
                    worldData.structureMap[x][y] = 1; // Water, Player cant enter
                    break;
                }
                default: {
                    worldData.structureMap[x][y] = 0; // Empty, Player can enter
                    break;
                }
            }
        }
    }
    delete generaceMapy;
}
