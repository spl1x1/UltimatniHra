//
// Created by Lukáš Kaplánek on 24.10.2025.
//
#include "WorldStructs.h"
#include "../../MACROS.h"


void WorldData::dealocateMap(int** map) {
    for (int i = 0; i < MAPSIZE; ++i) {
        delete map[i];
    }
    delete map;
}
void WorldData::dealocateMap(Structure*** map) {
    for (int i = 0; i < MAPSIZE; ++i) {
        delete map[i];
    }
    delete map;
}

WorldData::WorldData(){
    // Allocate memory for 2D arrays
    biomeMap = new int*[MAPSIZE];
    blockVariantionMap = new int*[MAPSIZE];
    structureMap = new Structure **[MAPSIZE];
    entityMap = new int*[MAPSIZE];
    for (int i = 0; i < MAPSIZE; ++i) {
        biomeMap[i] = new int[MAPSIZE];
        blockVariantionMap[i] = new int[MAPSIZE];
        structureMap[i] = new Structure*[MAPSIZE];
        entityMap[i] = new int[MAPSIZE];
    }
}

WorldData::~WorldData() {
    delete waterStructure;
    delete noStructure;

    dealocateMap(biomeMap);
    dealocateMap(blockVariantionMap);
    dealocateMap(entityMap);
    dealocateMap(structureMap);
}

