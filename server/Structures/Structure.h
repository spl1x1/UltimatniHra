//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#ifndef ULTIMATNIHRA_STRUCTURE_H
#define ULTIMATNIHRA_STRUCTURE_H
#include <memory>

enum class structureType{
    HOUSE,
    FARM,
    BARRACKS,
    TOWER
};

class IStructure {
public:
    virtual ~IStructure() = default;
    [[nodiscard]] virtual structureType getType() const = 0;
    [[nodiscard]] virtual int getId() const = 0;
};

class StructureHitbox {
    std::shared_ptr<class Server> server;

};

class Structure{
    int id;
    structureType type;
    std::unique_ptr<class Sprite> sprite;

    public:
    //Getters
    [[nodiscard]] int getId() const { return id; }
    [[nodiscard]] structureType getType() const { return type; }

    //Constructors
    Structure(int id, structureType type, std::unique_ptr<class Sprite> sprite= nullptr);
};


#endif //ULTIMATNIHRA_STRUCTURE_H