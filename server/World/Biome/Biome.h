//
// Created by Lukáš Kaplánek on 30.10.2025.
//

#ifndef BIOME_H
#define BIOME_H
#include <string>
#include <vector>
#include <SDL3/SDL_surface.h>


struct BiomeStructure {
    std::string structureType;
    std::string assetPath;
    double chance = 0.0;
};

enum BiomeType {
    NONE = -1,
    WATER = 0,
    BEACH = 1,
    DESERT = 2,
    GRASSLAND = 3,
    MOUNTAIN = 4,
    FOREST = 5,
    SNOW = 6
};

class Biome {
    static std::vector<std::unique_ptr<Biome>> registeredBiomesList;

protected:
    BiomeType biomeType = BiomeType::NONE;
    std::string assetPath = "NoTexture.bmp";
    int variationLevel = 0;
    int uniqueId = -1;
    int biomeSize = 0;
    bool hasStructures = false;
    std::vector<BiomeStructure> structures;

public:
    [[nodiscard]] BiomeType getBiomeType() const {;
        return biomeType;
    }
    std::string getAssetPath() {
        return assetPath;
    };
    int getVariationLevel() const {
        return variationLevel;
    }
    virtual SDL_Rect getTileVariantion(int variantionIndex);
    int getUniqueId() const {
        return uniqueId;
    }
    static void registerBiome(std::unique_ptr<Biome> biome){
        registeredBiomesList.push_back(std::move(biome));
    }
    static std::unique_ptr<Biome> getBiomeById(const int ID){
        if (ID < 0 || ID >= registeredBiomesList.size()) return nullptr;
        return std::move(registeredBiomesList.at(ID));
    }

    virtual ~Biome() = default;
};


#endif //BIOME_H
