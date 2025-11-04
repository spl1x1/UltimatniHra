//
// Created by Lukáš Kaplánek on 03.11.2025.
//

#ifndef PLAYERSPRITE_H
#define PLAYERSPRITE_H
#include "Sprite.h"


class PlayerSprite : Sprite {
public:
    PlayerSprite() {
        textureName = "player";
        activeAnimation = IDLE;
        direction = DOWN;
    }
};



#endif //PLAYERSPRITE_H
