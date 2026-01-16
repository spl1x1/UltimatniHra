//
// Created by Lukáš Kaplánek on 16.01.2026.
//

#ifndef ULTIMATNIHRA_SLIMESPRITE_H
#define ULTIMATNIHRA_SLIMESPRITE_H
#include "Sprite.hpp"

class SlimeSprite final : public ISprite{
    SpriteRenderingContext renderingContext = SpriteRenderingContext("Slime1",0.2f, 64, 64, Direction::DOWN, AnimationType::IDLE);
public:
    //Interface Methods
    void Tick(float deltaTime) override;
    std::tuple<float,int> GetFrameTimeAndCount() override;


    //Setters
    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override;
    void SetVariant(int newVariant) override {};
    void SetCurrentFrame(int newCurrentFrame) override{};

    //Getters
    std::tuple<std::string,SDL_FRect*> GetFrame() override;
    [[nodiscard]] int GetWidth() const override;
    [[nodiscard]] int GetHeight() const override;
    RenderingContext GetRenderingContext() override;
};

#endif //ULTIMATNIHRA_SLIME_H