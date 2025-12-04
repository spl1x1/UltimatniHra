//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H
#include "../Structures/Structure.h"


class WorldData {
    static void dealocateMap(int** map) ;
    static void dealocateMap(Structure*** map) ;


public:
    Structure* waterStructure = new Structure{Structure::WATER};
    Structure* noStructure = new Structure{Structure::NONE};
    
    int** biomeMap; // 2D array for biome map
    int** blockVariantionMap; // 2D array for block variation map
    Structure*** structureMap; // 2D array for structure map
    int** entityMap; // 2D array for entity map


    WorldData();
    ~WorldData();
};

#endif //WORLDDATASTRUCT_H
