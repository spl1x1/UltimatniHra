//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/PlayerSprite.hpp"

void PlayerSprite::Tick(float deltaTime) {
    renderingContext.Tick(deltaTime);
}

void PlayerSprite::setDirection(Direction newDirection) {
    renderingContext.setDirection(newDirection);
}
void PlayerSprite::setAnimation(AnimationType newAnimation) {
    renderingContext.setActiveAnimation(newAnimation);
}

std::tuple<std::string, SDL_FRect*> PlayerSprite::getFrame() {
    std::string texture = renderingContext.getTexture();
    texture = renderingContext.attachActiveAnimation(texture);
    texture = renderingContext.attachActiveDirection(texture);
    return  {texture, renderingContext.getFrameRect()};
}

int PlayerSprite::getWidth() const {
    return renderingContext.getWidth();
}

int PlayerSprite::getHeight() const {
    return renderingContext.getHeight();
}

RenderingContext PlayerSprite::getRenderingContext() {
    RenderingContext context;

    auto frame  = getFrame();
    context.textureName = std::get<0>(frame);
    context.rect = std::get<1>(frame);

    return context;
}

