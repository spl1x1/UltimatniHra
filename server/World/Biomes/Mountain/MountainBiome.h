//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef MOUNTAINBIOME_H
#define MOUNTAINBIOME_H
#include "../Biome.h"



class MountainBiome : public Biome {

    BiomeType _type = BiomeType::Mountain;
    float _temperature = 0.0f;

    int _biomeId = 0;
    std::string _assetPath = "mountain.bmp";
    int _variationLevels = 8;

public:
    ~MountainBiome() override = default;
    SDL_Rect GetAssetRect(int VariantionId) override;

};



#endif //MOUNTAINBIOME_H
