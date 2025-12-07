//
// Created by Lukáš Kaplánek on 24.10.2025.
//
#include "WorldStructs.h"
#include "../../MACROS.h"

void WorldData::updateMapValue(int x, int y, MapType mapType, int newValue) const {
    int step = x*MAPSIZE;
    switch (mapType) {
        case BIOME_MAP:
            biomeMap[step + y]= newValue;
            break;
        case BLOCK_VARIATION_MAP:
            blockVariantionMap[step + y] = newValue;
            break;
        case COLLISION_MAP:
            collisionMap[step + y] = newValue;
            break;
    }
}

int WorldData::getMapValue(int x, int y, MapType mapType) const {
    int step = x*MAPSIZE;
    switch (mapType) {
        case BIOME_MAP:
            return biomeMap[step + y];
        case BLOCK_VARIATION_MAP:
            return blockVariantionMap[step + y];
        case COLLISION_MAP:
            return collisionMap[step + y];
        default:
            return -1; // Invalid map type
    }
}

WorldData::WorldData(){
    // Allocate memory for arrays
    biomeMap = new int[MAPSIZE*MAPSIZE];
    blockVariantionMap = new int[MAPSIZE*MAPSIZE];
    collisionMap = new int[MAPSIZE*MAPSIZE];
}

WorldData::~WorldData() {
    delete[] biomeMap;
    delete[] blockVariantionMap;
    delete[] collisionMap;
}

