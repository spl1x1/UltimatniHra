//
// Created by USER on 09.12.2025.
//

#include "../../include/Sprites/ChestSprite.h"


std::tuple<std::string, SDL_FRect> ChestSprite::GetFrame() {
    return {renderingContext.GetTexture(), renderingContext.GetFrameRect()};
}

RenderingContext ChestSprite::GetRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.GetTexture();
    context.rect = renderingContext.GetFrameRect();
    return context;
}

void ChestSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> ChestSprite::GetFrameTimeAndCount() {
    return {0.2f, renderingContext.GetCurrentFrameCount()};
}

void ChestSprite::PlayAnimation(const AnimationType newAnimation, const Direction direction, const bool ForceReset) {
    renderingContext.PlayAnimation(newAnimation, direction, ForceReset);
}

void ChestSprite::SetVariant(const int newVariant) {
    renderingContext.SetVariant(newVariant);
}

int ChestSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int ChestSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

SpriteRenderingContext * ChestSprite::GetSpriteRenderingContext() { return &renderingContext; }
