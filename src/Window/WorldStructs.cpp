//
// Created by Lukáš Kaplánek on 24.10.2025.
//
#include "../../include/Window/WorldStructs.h"
#include "../../include/Application/MACROS.h"
#include "../../include/Application/dataStructures.h"


std::vector<Coordinates> WorldData::GetWaterTiles() const {
    std::vector<Coordinates> waterTiles;
    waterTiles.reserve(MAPSIZE * MAPSIZE / 10);
    for (int x = 0; x < MAPSIZE; ++x) {
        for (int y = 0; y < MAPSIZE; ++y) {
            if (GetMapValue(x,y, COLLISION_MAP) == -1) { // Assuming 1 represents water
                waterTiles.emplace_back(Coordinates{static_cast<float>(x), static_cast<float>(y)});
            }
        }
    }
    waterTiles.shrink_to_fit();
    return waterTiles;
}

void WorldData::ResetMaps() {
    std::fill(biomeMap.begin(), biomeMap.end(), 0);
    std::fill(blockVariantionMap.begin(), blockVariantionMap.end(), 0);
    std::fill(collisionMap.begin(), collisionMap.end(), 0);
}

void WorldData::UpdateMapValue(const int x, const int y, const MapType mapType, const int newValue) {
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

int WorldData::GetMapValue(int x, int y, const MapType mapType) const {
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

