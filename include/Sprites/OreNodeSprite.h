//
// Created by USER on 03.01.2026.
//

#ifndef ORENODESPRITE_H
#define ORENODESPRITE_H
#include "Sprite.hpp"

class OreNodeSprite : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("ore_nodes",  Direction::OMNI,0.0f, 2, 96, 96, 3);
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

#endif //ORENODESPRITE_H
