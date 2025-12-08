//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef WORLDDATASTRUCT_H
#define WORLDDATASTRUCT_H


class WorldData {
    //Zmena na 1D pole, step zname je to MAPSIZE tak usetrime pamet
    int* biomeMap;
    int* blockVariantionMap;
    int* collisionMap;

public:
    enum MapType {
        BIOME_MAP,
        BLOCK_VARIATION_MAP,
        COLLISION_MAP
    };


    void updateMapValue(int x, int y, MapType mapType, int newValue) const;
    [[nodiscard]] int getMapValue(int x, int y, MapType mapType) const;

    WorldData();
    ~WorldData();
};

#endif //WORLDDATASTRUCT_H
