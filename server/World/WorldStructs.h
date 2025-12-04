//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H


class WorldData {
    static void dealocateMap(int** map) ;


public:
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** collisionMap; // 2D array for structure map


    WorldData();
    ~WorldData();
};

#endif //WORLDDATASTRUCT_H
