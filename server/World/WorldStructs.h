//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H
#include <vector>


class WorldData {
    void dealocateMap(int** map) const;

public:
    int seed;
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** structureMap; // 2D array for structure map

    WorldData(int seed = 0);

    static void getBlockVariationMap(const WorldData& worldData);
    static void getStructureMap(const WorldData& worldData);
};

#endif //WORLDDATASTRUCT_H
