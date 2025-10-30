//
// Created by Lukáš Kaplánek on 30.10.2025.
//

#include "WaterBiome.h"

SDL_Rect WaterBiome::getTileVariantion(int variantionIndex) {
    int x = 0 + variantionIndex*32;
    SDL_Rect tileVariant = {x,0,32,32};
    return tileVariant;
};

