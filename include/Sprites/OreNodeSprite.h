//
// Created by USER on 03.01.2026.
//

#ifndef ORENODESPRITE_H
#define ORENODESPRITE_H
#include "Sprite.hpp"

class OreNodeSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("ore_nodes",0.0,96, 96);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override {}
    std::tuple<float,int> GetFrameTimeAndCount() override;

    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override {};
    void SetVariant(int newVariant) override; //Sets ore type (iron, copper, gold)
    void SetCurrentFrame(int newCurrentFrame) override;

    std::tuple<std::string,SDL_FRect*> GetFrame() override;
    RenderingContext GetRenderingContext() override;
    [[nodiscard]] int GetWidth() const override;
    [[nodiscard]] int GetHeight() const override;
    SpriteRenderingContext* GetSpriteRenderingContext() override;
};

#endif //ORENODESPRITE_H
