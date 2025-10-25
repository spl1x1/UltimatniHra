//
// Created by Jar Jar Banton on 19. 10. 2025.
//

#include "generace_mapy.h"
#include "WorldStructs.h"

#ifndef MAP_WIDTH
#define MAP_WIDTH 512
#endif
#ifndef MAP_HEIGHT
#define MAP_HEIGHT 512
#endif
#ifndef VARIATION_LEVELS
#define VARIATION_LEVELS 5
#endif

int main() {
    GeneraceMapy mapa;
    const auto& map = mapa.getBiomMapa();
    // for (int y = 0; y < GeneraceMapy::MAP_HEIGHT; y++) {
    //     for (int x = 0; x < GeneraceMapy::MAP_WIDTH; x++) {
    //         std::cout << map[x][y] << " ";
    //     }
    //     std::cout << "\n";
    // }
    WorldData worldData = WorldData(42);
    WorldData::getBlockVariationMap(worldData);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            std::cout << worldData.blockVariantionMap[x][y] << " ";
        }
        std::cout << "\n";
    }
    return 0;
}
