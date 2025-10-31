//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef FORESTBIOME_H
#define FORESTBIOME_H
#include "../Biome.h"



class ForestBiome : public Biome {

    BiomeType _type = BiomeType::Forest;
    float _temperature = 0.0f;

    int _biomeId = 0;
    std::string _assetPath = "forest.bmp";
    int _variationLevels = 8;

public:
    ~ForestBiome() override = default;
    SDL_Rect GetAssetRect(int VariantionId) override;

};



#endif //FORESTBIOME_H
