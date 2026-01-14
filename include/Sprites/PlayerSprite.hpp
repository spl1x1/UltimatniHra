//
// Created by Lukáš Kaplánek on 03.11.2025.
//

#ifndef PLAYERSPRITE_H
#define PLAYERSPRITE_H

#include "Sprite.hpp"

class PlayerSprite final : public ISprite {
    SpriteRenderingContext renderingContext = SpriteRenderingContext("assets/jsons/entities/player.json","player",0.1f, 96, 96, Direction::DOWN, AnimationType::IDLE);
public:
    //Interface Methods
    void Tick(float deltaTime) override;


    //Setters
    void PlayAnimation(AnimationType newAnimation, Direction direction, bool ForceReset) override;
    void setVariant(int newVariant) override {}
    void setCurrentFrame(int newCurrentFrame) override{};

    //Getters
    std::tuple<std::string,SDL_FRect*> getFrame() override;
    [[nodiscard]] int getWidth() const override;
    [[nodiscard]] int getHeight() const override;
    RenderingContext getRenderingContext() override;
};



#endif //PLAYERSPRITE_H
