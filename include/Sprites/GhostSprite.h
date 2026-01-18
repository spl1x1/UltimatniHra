//
// Created by USER on 18.01.2026.
//

#ifndef GHOSTSPRITE_H
#define GHOSTSPRITE_H

//
// Created by Lukáš Kaplánek on 03.11.2025.
//

#include "Sprite.hpp"

class GhostSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("ghost",0.1f, 96, 96, Direction::DOWN, AnimationType::IDLE);
public:
    //Interface Methods
    void Tick(float deltaTime) override;
    std::tuple<float,int> GetFrameTimeAndCount() override;


    //Setters
    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override;
    void SetVariant(int newVariant) override {}
    void SetCurrentFrame(int newCurrentFrame) override{};

    //Getters
    std::tuple<std::string,SDL_FRect*> GetFrame() override;
    [[nodiscard]] int GetWidth() const override;
    [[nodiscard]] int GetHeight() const override;
    RenderingContext GetRenderingContext() override;
    SpriteRenderingContext* GetSpriteRenderingContext() override;
};



#endif //GHOSTSPRITE_H

