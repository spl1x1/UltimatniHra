//
// Created by Lukáš Kaplánek on 20.01.2026.
//

#ifndef CHESTSPRITE_H
#define CHESTSPRITE_H
//
// Created by USER on 09.12.2025.
//
#include "Sprite.hpp"

class ChestSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("chest",0.3f, 32, 64, Direction::DOWN, AnimationType::IDLE);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override;
    std::tuple<float,int> GetFrameTimeAndCount() override;

    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override;;
    void SetVariant(int newVariant) override;
    void SetCurrentFrame(int newCurrentFrame) override{};

    std::tuple<std::string,SDL_FRect> GetFrame() override;
    RenderingContext GetRenderingContext() override;
    [[nodiscard]] int GetWidth() const override;
    [[nodiscard]] int GetHeight() const override;
    SpriteRenderingContext* GetSpriteRenderingContext() override;
};


#endif //CHESTSPRITE_H