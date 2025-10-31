//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef SNOWBIOME_H
#define SNOWBIOME_H
#include "../Biome.h"



class WaterBiome : public Biome {

    BiomeType _type = BiomeType::Snow;
    float _temperature = 0.0f;

    int _biomeId = 0;
    std::string _assetPath = "snow.bmp";
    int _variationLevels = 8;

public:
    ~WaterBiome() override = default;
    SDL_Rect GetAssetRect(int VariantionId) override;

};



#endif //SNOWBIOME_H
