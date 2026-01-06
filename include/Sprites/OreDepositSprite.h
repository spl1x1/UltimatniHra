//
// Created by Lukáš Kaplánek on 06.01.2026.
//

#ifndef ULTIMATNIHRA_OREDEPOSITSPRITE_H
#define ULTIMATNIHRA_OREDEPOSITSPRITE_H
#include "Sprite.hpp"

//
// Created by USER on 03.01.2026.
//

class OreDepositSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("ore_deposits",  Direction::OMNI,0.0f, 1, 96, 96, 3);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override {};

    void setDirection(Direction newDirection)  override {};
    void setAnimation(AnimationType newAnimation) override {};
    void setVariant(int newVariant) override; //Sets ore type (iron, copper, gold)
    void setCurrentFrame(int newCurrentFrame) override;

    std::tuple<std::string,SDL_FRect*> getFrame() override;
    RenderingContext getRenderingContext() override;
    [[nodiscard]] int getWidth() const override;
    [[nodiscard]] int getHeight() const override;
};

#endif //ULTIMATNIHRA_OREDEPOSITSPRITE_H