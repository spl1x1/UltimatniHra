//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H
#include <vector>


struct WorldData {
    int seed;
    // Add other world-related data members as needed
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** structureMap; // 2D array for structure map


    std::vector<std::vector<int>> WorldMap; // placeholder for world map TODO: vyměnit za var mapy výše

    void dealocateMap(int** map);

    WorldData(int seed = 0);

    static void getBlockVariationMap(WorldData worldData);
};

#endif //WORLDDATASTRUCT_H
