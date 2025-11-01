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

WorldData::WorldData(){
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

