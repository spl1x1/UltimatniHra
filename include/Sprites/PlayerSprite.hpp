//
// Created by Lukáš Kaplánek on 03.11.2025.
//

#ifndef PLAYERSPRITE_H
#define PLAYERSPRITE_H
//#include <RmlUi/Core/Spritesheet.h>

#include "Sprite.hpp"
#include "../Server/BossPathfinding.h"

class PlayerSprite : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("player",Direction::DOWN ,0.1f, 8, 96, 96, 0.0f, 0.0f);
public:
    //Interface Methods
    void Tick(float deltaTime) override;


    //Setters
    void setDirection(Direction newDirection) override;
    void setAnimation(AnimationType newAnimation) override;

    //Getters
    std::tuple<std::string,SDL_FRect*> getFrame() override;
    [[nodiscard]] int getWidth() const override;
    [[nodiscard]] int getHeight() const override;
    RenderingContext getRenderingContext() override;
};



#endif //PLAYERSPRITE_H
