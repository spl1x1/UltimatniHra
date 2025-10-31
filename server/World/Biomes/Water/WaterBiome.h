//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef WATERBIOME_H
#define WATERBIOME_H
#include "../Biomes.h"



class WaterBiome : public Biome {

    BiomeType _type = BiomeType::Water;
    float _temperature = 0.0f;

    int _biomeId = 0;
    std::string _assetPath = "water.bmp";
    int _variationLevels = 8;

public:
    ~WaterBiome() override = default;
    SDL_Rect GetAssetRect(int VariantionId) override;

};



#endif //WATERBIOME_H
