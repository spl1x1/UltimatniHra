//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef DESERTBIOME_H
#define DESERTBIOME_H
#include "../Biome.h"



class DesertBiome : public Biome {

    BiomeType _type = BiomeType::Desert;
    float _temperature = 0.0f;

    int _biomeId = 0;
    std::string _assetPath = "desert.bmp";
    int _variationLevels = 8;

public:
    ~DesertBiome() override = default;
    SDL_Rect GetAssetRect(int VariantionId) override;

};



#endif //DESERTBIOME_H
