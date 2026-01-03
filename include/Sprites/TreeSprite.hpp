//
// Created by USER on 09.12.2025.
//

#ifndef TREESPRITE_HPP
#define TREESPRITE_HPP
#include "Sprite.hpp"

class TreeSprite : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("tree",  Direction::OMNI,.2f, 16, 96, 64, 3);
public:
    //Interface Methods Implementation
    void Tick(float deltaTime) override;

    void setDirection(Direction newDirection)  override {};
    void setAnimation(AnimationType newAnimation) override {};
    void setVariant(int newVariant) override;
    void setCurrentFrame(int newCurrentFrame) override{};

    std::tuple<std::string,SDL_FRect*> getFrame() override;
    RenderingContext getRenderingContext() override;
    [[nodiscard]] int getWidth() const override;
    [[nodiscard]] int getHeight() const override;
};

#endif //TREESPRITE_HPP
