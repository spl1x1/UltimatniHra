//
// Created by USER on 03.01.2026.
//

#include "../../include/Sprites/OreNodeSprite.h"

std::tuple<float, int> OreNodeSprite::GetFrameTimeAndCount() {
    return {0.0f,1};
}

void OreNodeSprite::SetVariant(const int newVariant) {
    renderingContext.SetVariant(newVariant);
}

void OreNodeSprite::SetCurrentFrame(const int newCurrentFrame) {
    renderingContext.SetCurrentFrame(newCurrentFrame);
}

std::tuple<std::string, SDL_FRect*> OreNodeSprite::GetFrame() {
    return {renderingContext.GetTexture(), renderingContext.GetFrameRect()};
}

RenderingContext OreNodeSprite::GetRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.GetTexture();
    context.rect = renderingContext.GetFrameRect();
    return context;
}

int OreNodeSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int OreNodeSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

SpriteRenderingContext * OreNodeSprite::GetSpriteRenderingContext() {
    return &renderingContext;
}
