//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#ifndef ULTIMATNIHRA_STRUCTURE_H
#define ULTIMATNIHRA_STRUCTURE_H


class Structure {
    public:
    enum Type {
        NONE = 0,
        DEPLOYABLE,
        WATER
    };
    Type type;
};


#endif //ULTIMATNIHRA_STRUCTURE_H