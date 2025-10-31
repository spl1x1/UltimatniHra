//
// Created by Lukáš Kaplánek on 31.10.2025.
//

#ifndef STRUCTURE_H
#define STRUCTURE_H
#include <string>
#include <tuple>
#include <SDL3/SDL_rect.h>

enum StructureType {
    Empty = -1,
    House = 0,
    Tree = 1,
    Rock = 2
};

class Structure {
protected:
    int structureId = -1;
    int width = 0;
    int height = 0;
    int _variationLevels = 1;

    StructureType structureType = StructureType::Empty;
    std::string name;
    std::string assetPath;
public:

    virtual ~Structure() = default;
    virtual SDL_Rect GetAssetRect(int VariantionId){return {};};

    [[nodiscard]] int GetVariationLevels() const {return _variationLevels;}

    [[nodiscard]] std::tuple<int,int> GetSize() const {
        return {width/32, height/32};
    }

    [[nodiscard]] int GetStructureId() const {
        return structureId;
    }

    [[nodiscard]] int GetWidth() const {
        return width;
    }

    [[nodiscard]] int GetHeight() const {
        return height;
    }

    [[nodiscard]] StructureType GetStructureType() const {
        return structureType;
    }

    [[nodiscard]] std::string GetName() const {
        return name;
    }

    [[nodiscard]] std::string GetAssetPath() const {
        return assetPath;
    }

};


#endif //STRUCTURE_H
