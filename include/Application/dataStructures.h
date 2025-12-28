//
// Created by Lukáš Kaplánek on 08.12.2025.
//

#ifndef ULTIMATNIHRA_DATASTRUCTURES_H
#define ULTIMATNIHRA_DATASTRUCTURES_H

//Defines a 2D coordinate
struct Coordinates {
    float x = 0;
    float y = 0;
};

struct CollisionStatus{
    bool colliding;
    bool collisionDisabled;
};
#endif //ULTIMATNIHRA_DATASTRUCTURES_H
