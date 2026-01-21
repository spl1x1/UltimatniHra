//
// Created by USER on 09.12.2025.
//

#include "../../include/Sprites/AnchorSprite.h"


std::tuple<std::string, SDL_FRect> AnchorSprite::GetFrame() {
    return {renderingContext.GetTexture(), renderingContext.GetFrameRect()};
}

RenderingContext AnchorSprite::GetRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.GetTexture();
    context.rect = renderingContext.GetFrameRect();
    return context;
}

void AnchorSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> AnchorSprite::GetFrameTimeAndCount() {
    return {0.2f, renderingContext.GetCurrentFrameCount()};
}

void AnchorSprite::PlayAnimation(const AnimationType newAnimation, const Direction direction, const bool ForceReset) {
    renderingContext.PlayAnimation(newAnimation, direction, ForceReset);
}

void AnchorSprite::SetVariant(const int newVariant) {
    renderingContext.SetVariant(newVariant);
}

int AnchorSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int AnchorSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

SpriteRenderingContext * AnchorSprite::GetSpriteRenderingContext() { return &renderingContext; }
