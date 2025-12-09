//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef WATERSPRITE_H
#define WATERSPRITE_H
#include <memory>
#include "../../include/Sprites/Sprite.hpp"

class WaterSprite : public Sprite {

public:
    WaterSprite();
    std::tuple<std::string,SDL_FRect*> getFrame() override;
};



#endif //WATERSPRITE_H
