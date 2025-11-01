//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef WATERSPRITE_H
#define WATERSPRITE_H
#include "Sprite.h"


class WaterSprite : public Sprite {

public:
    WaterSprite() {
        frameDuration = 0.2f; // 5 FPS
        frameCount = 4;
        texturePrefix = "water";

    }

};



#endif //WATERSPRITE_H
