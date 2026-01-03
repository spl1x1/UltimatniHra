//
// Created by USER on 03.01.2026.
//

#include "../../include/Sprites/OreNodeSprite.h"

void OreNodeSprite::setVariant(int newVariant) {
    renderingContext.setVariant(newVariant);
}

void OreNodeSprite::setCurrentFrame(int newCurrentFrame) {
    renderingContext.setCurrentFrame(newCurrentFrame);
}

std::tuple<std::string, SDL_FRect*> OreNodeSprite::getFrame() {
    return {renderingContext.getTexture(), renderingContext.getFrameRect()};
}

RenderingContext OreNodeSprite::getRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.getTexture();
    context.rect = renderingContext.getFrameRect();
    return context;
}

int OreNodeSprite::getWidth() const {
    return renderingContext.getWidth();
}

int OreNodeSprite::getHeight() const {
    return renderingContext.getHeight();
}