//
// Created by USER on 09.12.2025.
//

#include "../../include/Sprites/TreeSprite.hpp"


std::tuple<std::string, SDL_FRect*> TreeSprite::getFrame() {
    return {renderingContext.getTexture(), renderingContext.getFrameRect()};
}

RenderingContext TreeSprite::getRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.getTexture();
    context.rect = renderingContext.getFrameRect();
    return context;
}

void TreeSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

void TreeSprite::setVariant(const int newVariant) {
    renderingContext.setVariant(newVariant);
}

int TreeSprite::getWidth() const {
    return renderingContext.getWidth();
}

int TreeSprite::getHeight() const {
    return renderingContext.getHeight();
}
