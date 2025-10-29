//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef MAP_WIDTH
#define MAP_WIDTH 512
#endif
#ifndef MAP_HEIGHT
#define MAP_HEIGHT 512
#endif
#ifndef VARIATION_LEVELS
#define VARIATION_LEVELS 5
#endif

#include "WorldStructs.h"

#include <random>

void WorldData::dealocateMap(int** map) const {
    for (int i = 0; i < MAP_WIDTH; ++i) {
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
    biomeMap = new int*[MAP_WIDTH];
    blockVariantionMap = new int*[MAP_WIDTH];
    structureMap = new int*[MAP_WIDTH];
    for (int i = 0; i < MAP_WIDTH; ++i) {
        biomeMap[i] = new int[MAP_HEIGHT];
        blockVariantionMap[i] = new int[MAP_HEIGHT];
        structureMap[i] = new int[MAP_HEIGHT];
    }
}

void WorldData::getBlockVariationMap(const WorldData& worldData) {
    std::srand(static_cast<unsigned int>(worldData.seed));
    std::mt19937 mt(worldData.seed);
    std::uniform_real_distribution<double> dist(1.0,(double) VARIATION_LEVELS);

    for (int i = 0; i < MAP_WIDTH; ++i) {
        for (int j = 0; j < MAP_HEIGHT; ++j) {
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



    for (int i = 0; i < MAP_WIDTH; ++i) {
        for (int j = 0; j < MAP_HEIGHT; ++j) {
            double chance = dist(mt);
            if (chance < 0.05) {
                worldData.structureMap[i][j] = 1;
            } else {
                worldData.structureMap[i][j] = 0;
            }
        }
    }
}
