//
// Created by USER on 09.12.2025.
//

#ifndef TREESPRITE_HPP
#define TREESPRITE_HPP
#include "Sprite.hpp"

class TreeSprite : public Sprite {

public:
    TreeSprite();
    std::tuple<std::string,std::shared_ptr<SDL_FRect>> getFrame() override;
};

#endif //TREESPRITE_HPP
