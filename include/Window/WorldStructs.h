//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H
#include <vector>


class WorldData {
    //Zmena na 1D pole, step zname je to MAPSIZE tak usetrime pamet
    std::vector<int> biomeMap;
    std::vector<int> blockVariantionMap;
    std::vector<int> collisionMap;

public:
    enum MapType {
        BIOME_MAP,
        BLOCK_VARIATION_MAP,
        COLLISION_MAP
    };


    void updateMapValue(int x, int y, MapType mapType, int newValue);
    [[nodiscard]] int getMapValue(int x, int y, MapType mapType) const;

    WorldData();
};

#endif //WORLDDATASTRUCT_H
