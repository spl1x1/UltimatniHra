//
// Created by USER on 03.01.2026.
//

#ifndef ORENODESPRITE_H
#define ORENODESPRITE_H
#include "Sprite.hpp"

class OreNodeSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("assets/jsons/structures/ore_nodes.json","ore_nodes",0.0,96, 96);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override {};

    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override {};
    void setVariant(int newVariant) override; //Sets ore type (iron, copper, gold)
    void setCurrentFrame(int newCurrentFrame) override;

    std::tuple<std::string,SDL_FRect*> getFrame() override;
    RenderingContext getRenderingContext() override;
    [[nodiscard]] int getWidth() const override;
    [[nodiscard]] int getHeight() const override;
};

#endif //ORENODESPRITE_H
