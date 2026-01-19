//
// Created by Lukáš Kaplánek on 24.10.2025.
//
#include "../../include/Window/WorldStructs.h"
#include "../../include/Application/MACROS.h"


void WorldData::updateMapValue(int x, int y, MapType mapType, int newValue) {
    int step = x*MAPSIZE;
    switch (mapType) {
        case BIOME_MAP:
            biomeMap.at(step + y)= newValue;
            break;
        case BLOCK_VARIATION_MAP:
            blockVariantionMap.at(step + y) = newValue;
            break;
        case COLLISION_MAP:
            collisionMap.at(step + y) = newValue;
            break;
    }
}

int WorldData::getMapValue(int x, int y, const MapType mapType) const {
    int step = x*MAPSIZE;
    switch (mapType) {
        case BIOME_MAP:
            return biomeMap.at(step + y);
        case BLOCK_VARIATION_MAP:
            return blockVariantionMap.at(step + y);
        case COLLISION_MAP:
            return collisionMap.at(step + y);
        default:
            return -1; // Invalid map type
    }
}

WorldData::WorldData(){
    // Allocate memory for arrays
    biomeMap.resize(MAPSIZE*MAPSIZE);
    blockVariantionMap.resize(MAPSIZE*MAPSIZE);
    collisionMap.resize(MAPSIZE*MAPSIZE);
}

