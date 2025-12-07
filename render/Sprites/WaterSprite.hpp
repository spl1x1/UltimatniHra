//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef WATERSPRITE_H
#define WATERSPRITE_H
#include "Sprite.hpp"


class WaterSprite : public Sprite {

public:
    WaterSprite();

    std::tuple<std::string,std::shared_ptr<SDL_FRect>> getFrame() override;
};



#endif //WATERSPRITE_H
