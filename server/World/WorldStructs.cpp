//
// Created by Lukáš Kaplánek on 24.10.2025.
//
#include "WorldStructs.h"
#include "../../MACROS.h"

#include <random>


void WorldData::dealocateMap(int** map) const {
    for (int i = 0; i < MAPSIZE; ++i) {
        delete[] biomeMap[i];
        delete[] blockVariantionMap[i];
        delete[] structureMap[i];
    }
    delete[] biomeMap;
    delete[] blockVariantionMap;
    delete[] structureMap;
}

WorldData::WorldData(int seed){
    this->seed = seed;
    // Allocate memory for 2D arrays
    biomeMap = new int*[MAPSIZE];
    blockVariantionMap = new int*[MAPSIZE];
    structureMap = new int*[MAPSIZE];
    for (int i = 0; i < MAPSIZE; ++i) {
        biomeMap[i] = new int[MAPSIZE];
        blockVariantionMap[i] = new int[MAPSIZE];
        structureMap[i] = new int[MAPSIZE];
    }
}

//asi by to chtělo přejmenovat na generate místo get - zapisuje hodnotu přímo do proměnné objektu
void WorldData::getBlockVariationMap(const WorldData& worldData) {
    std::srand(static_cast<unsigned int>(worldData.seed));
    std::mt19937 mt(worldData.seed);
    std::uniform_real_distribution<double> dist(1.0,VARIATION_LEVELS);

    for (int i = 0; i < MAPSIZE; ++i) {
        for (int j = 0; j < MAPSIZE; ++j) {
            int variation = static_cast<int>(dist(mt)); // Random variation level
            worldData.blockVariantionMap[i][j] = variation;
        }
    }
}


// TODO: Struktry zatím - strom, ore, voda,

void WorldData::getStructureMap(const WorldData& worldData) {
    std::srand(static_cast<unsigned int>(worldData.seed + 1)); // Different seed for structure map
    std::mt19937 mt(worldData.seed + 1);
    std::uniform_real_distribution<double> dist(0.0, 1.0);


    //TODO: Check based on biome type
    // zatím place holder
    for (int i = 0; i < MAPSIZE; ++i) {
        for (int j = 0; j < MAPSIZE; ++j) {
            double chance = dist(mt);
            /*if (chance < 0.05) {
                worldData.structureMap[i][j] = 1;
            } else {
                worldData.structureMap[i][j] = 0;
            } */




        }
    }
}
