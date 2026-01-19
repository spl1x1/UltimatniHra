//
// Created by Lukáš Kaplánek on 06.01.2026.
//

#include "../../include/Sprites/OreDepositSprite.h"

std::tuple<float, int> OreDepositSprite::GetFrameTimeAndCount() {
    return {0.0f,1};
}

void OreDepositSprite::SetVariant(const int newVariant) {
    renderingContext.SetVariant(newVariant);
}

void OreDepositSprite::SetCurrentFrame(const int newCurrentFrame) {
    renderingContext.SetCurrentFrame(newCurrentFrame);
}

std::tuple<std::string, SDL_FRect> OreDepositSprite::GetFrame() {
    return {renderingContext.GetTexture(), renderingContext.GetFrameRect()};
}

RenderingContext OreDepositSprite::GetRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.GetTexture();
    context.rect = renderingContext.GetFrameRect();
    return context;
}

int OreDepositSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int OreDepositSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

SpriteRenderingContext * OreDepositSprite::GetSpriteRenderingContext() {
    return &renderingContext;
}
