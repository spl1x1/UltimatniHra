//
// Created by Lukáš Kaplánek on 30.10.2025.
//

#ifndef WATERBIOME_H
#define WATERBIOME_H
#include "../Biome.h"


class WaterBiome : public Biome {
    std::string assetPath = "water.bmp";
    int uniqueId = 0;
    int variationLevel = 8;
    bool hasStructures = false;
public:
    SDL_Rect getTileVariantion(int variantionIndex) override;
};



#endif //WATERBIOME_H
