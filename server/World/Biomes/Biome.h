//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef BIOMES_H
#define BIOMES_H
#include <vector>
#include <SDL3/SDL_rect.h>
#include <string>
#include <tuple>
#include "Structure.h"

enum BiomeType {
    None = -1,
    Water = 0,
    Beach = 1,
    Desert = 2,
    Grassland = 3,
    Mountain = 4,
    Forest = 5,
    Snow = 6
};

class Biome {

protected:
    BiomeType _type = BiomeType::None;
    float _temperature = 0.0f;

    int _biomeId = -1;

    std::vector<std::tuple<int,int>> _blocks; // Lists coordinates of blocks belonging to this biome
    std::vector<Structure> _structures; // Lists structures belonging to this biome
    std::string _assetPath;
    int _variationLevels = 1;


public:

    virtual ~Biome() = default;

    virtual SDL_Rect GetAssetRect(int VariantionId){return {};};

    [[nodiscard]] int GetVariationLevels() const {return _variationLevels;}

    [[nodiscard]] std::string GetAssetPath() const {
        return _assetPath;
    }

    [[nodiscard]] BiomeType GetType() const {
        return _type;
    }
    [[nodiscard]] float GetTemperature() const {
        return _temperature;
    }
    [[nodiscard]] int GetBiomeId() const {
        return _biomeId;
    }

    [[nodiscard]] std::vector<Structure>  GetStructureList() const {
        return _structures;
    }

    [[nodiscard]] const std::vector<std::tuple<int, int>>& GetBlocks() const {
        return _blocks;
    }

};

#endif //BIOMES_H
