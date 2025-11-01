//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H


class WorldData {
    void dealocateMap(int** map) const;

public:
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** structureMap; // 2D array for structure map

    WorldData();
};

#endif //WORLDDATASTRUCT_H
