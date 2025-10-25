//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H


struct WorldData {
    int seed;
    // Add other world-related data members as needed
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** structureMap; // 2D array for structure map

    void dealocateMap(int** map);

    WorldData(int seed = 0);

    static void getBlockVariationMap(WorldData worldData);
};

#endif //WORLDDATASTRUCT_H
