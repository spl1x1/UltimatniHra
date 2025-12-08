//
// Created by Lukáš Kaplánek on 03.11.2025.
//

#ifndef PLAYERSPRITE_H
#define PLAYERSPRITE_H
#include "Sprite.hpp"

class PlayerSprite : public Sprite {
public:
    PlayerSprite();
    using Sprite::getFrame;
};



#endif //PLAYERSPRITE_H
