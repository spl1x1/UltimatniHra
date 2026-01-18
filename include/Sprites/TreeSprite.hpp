//
// Created by USER on 09.12.2025.
//

#ifndef TREESPRITE_HPP
#define TREESPRITE_HPP
#include "Sprite.hpp"

class TreeSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("tree",0.2f, 96, 64, Direction::OMNI, AnimationType::IDLE);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override;
    std::tuple<float,int> GetFrameTimeAndCount() override;

    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override {};
    void SetVariant(int newVariant) override;
    void SetCurrentFrame(int newCurrentFrame) override{};

    std::tuple<std::string,SDL_FRect*> GetFrame() override;
    RenderingContext GetRenderingContext() override;
    [[nodiscard]] int GetWidth() const override;
    [[nodiscard]] int GetHeight() const override;
    SpriteRenderingContext* GetSpriteRenderingContext() override;
};

#endif //TREESPRITE_HPP
