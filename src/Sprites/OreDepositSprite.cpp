//
// Created by Lukáš Kaplánek on 06.01.2026.
//

#include "../../include/Sprites/OreDepositSprite.h"

void OreDepositSprite::setVariant(const int newVariant) {
    renderingContext.setVariant(newVariant);
}

void OreDepositSprite::setCurrentFrame(const int newCurrentFrame) {
    renderingContext.setCurrentFrame(newCurrentFrame);
}

std::tuple<std::string, SDL_FRect*> OreDepositSprite::getFrame() {
    return {renderingContext.getTexture(), renderingContext.getFrameRect()};
}

RenderingContext OreDepositSprite::getRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.getTexture();
    context.rect = renderingContext.getFrameRect();
    return context;
}

int OreDepositSprite::getWidth() const {
    return renderingContext.getWidth();
}

int OreDepositSprite::getHeight() const {
    return renderingContext.getHeight();
}