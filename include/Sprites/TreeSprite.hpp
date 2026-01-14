//
// Created by USER on 09.12.2025.
//

#ifndef TREESPRITE_HPP
#define TREESPRITE_HPP
#include "Sprite.hpp"

class TreeSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("assets/jsons/structures/tree.json","tree",0.2f, 96, 64, Direction::OMNI, AnimationType::IDLE);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override;

    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override {};
    void setVariant(int newVariant) override;
    void setCurrentFrame(int newCurrentFrame) override{};

    std::tuple<std::string,SDL_FRect*> getFrame() override;
    RenderingContext getRenderingContext() override;
    [[nodiscard]] int getWidth() const override;
    [[nodiscard]] int getHeight() const override;
};

#endif //TREESPRITE_HPP
