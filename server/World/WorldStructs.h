//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H
#include <vector>
#include "../Entities/EntityStructs.h"


class WorldData {
    void dealocateMap(int** map) const;

public:
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    int** structureMap; // 2D array for structure map
    int** entityMap; // 2D array for entity map

    std::vector<Entity> entities;

    WorldData();
};

#endif //WORLDDATASTRUCT_H
