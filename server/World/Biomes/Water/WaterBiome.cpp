//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#include "WaterBiome.h"

SDL_Rect WaterBiome::GetAssetRect(int VariantionId) {
    if (VariantionId <= 0) {
        VariantionId = 1;
    }
        while (VariantionId > _variationLevels) {
            VariantionId-=_variationLevels;
        }
    SDL_Rect rect;
    rect.x = 0 + VariantionId-1 * 32;
    rect.y = 0;
    rect.w = 32;
    rect.h = 32;
    return rect;
}
