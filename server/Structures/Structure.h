//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#ifndef ULTIMATNIHRA_STRUCTURE_H
#define ULTIMATNIHRA_STRUCTURE_H

enum class structureType{
    HOUSE,
    FARM,
    BARRACKS,
    TOWER
};

class Structure {
    public:
    int id;
    structureType type;
};


#endif //ULTIMATNIHRA_STRUCTURE_H