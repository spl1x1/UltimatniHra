//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef GRASSBIOME_H
#define GRASSBIOME_H
#include "../Biome.h"



class GrasslandBiome : public Biome {

    BiomeType _type = BiomeType::Grassland;
    float _temperature = 0.0f;

    int _biomeId = 0;
    std::string _assetPath = "grassland.bmp";
    int _variationLevels = 8;

public:
    ~GrasslandBiome() override = default;
    SDL_Rect GetAssetRect(int VariantionId) override;

};



#endif //GRASSBIOME_H
