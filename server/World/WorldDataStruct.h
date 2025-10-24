//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H

#ifndef MAP_WIDTH
#define MAP_WIDTH 512
#endif
#ifndef MAP_HEIGHT
#define MAP_HEIGHT 512
#endif
#ifndef VARIATION_LEVELS
#define VARIATION_LEVELS 5
#endif


struct WorldDataStruct {
    int seed;
    // Add other world-related data members as needed
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** structureMap; // 2D array for structure map

    void dealocateMap(int** map);

    WorldDataStruct(int seed);

    static void getBlockVariationMap(WorldDataStruct worldData);
};

#endif //WORLDDATASTRUCT_H
