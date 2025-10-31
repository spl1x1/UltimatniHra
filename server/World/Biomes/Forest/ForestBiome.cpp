//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#include "ForestBiome.h"

SDL_Rect ForestBiome::GetAssetRect(int VariantionId) {
    if (VariantionId <= 0 || VariantionId >= _variationLevels) {
        VariantionId = 1;
    }
    SDL_Rect rect;
    rect.x = 0 + VariantionId-1 * 32;
    rect.y = 0;
    rect.w = 32;
    rect.h = 32;
    return rect;
}
