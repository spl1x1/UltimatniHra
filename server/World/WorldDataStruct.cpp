//
// Created by Lukáš Kaplánek on 24.10.2025.
//


#include "WorldDataStruct.h"
#include <cstdlib>

void WorldDataStruct::dealocateMap(int** map) {
    for (int i = 0; i < MAP_WIDTH; ++i) {
        delete[] biomeMap[i];
        delete[] blockVariantionMap[i];
        delete[] structureMap[i];
    }
    delete[] biomeMap;
    delete[] blockVariantionMap;
    delete[] structureMap;
}

WorldDataStruct::WorldDataStruct(int seed){
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

void WorldDataStruct::getBlockVariationMap(WorldDataStruct worldData) {
    std::srand(static_cast<unsigned int>(worldData.seed));
    for (int i = 0; i < MAP_WIDTH; ++i) {
        for (int j = 0; j < MAP_HEIGHT; ++j) {
            int variation = rand() % VARIATION_LEVELS; // Random variation level
            worldData.blockVariantionMap[i][j] = variation;
        }
    }
}
