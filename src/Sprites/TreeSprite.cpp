//
// Created by USER on 09.12.2025.
//

#include "../../include/Sprites/TreeSprite.hpp"


std::tuple<std::string, SDL_FRect*> TreeSprite::GetFrame() {
    return {renderingContext.GetTexture(), renderingContext.GetFrameRect()};
}

RenderingContext TreeSprite::GetRenderingContext() {
    RenderingContext context;
    context.textureName = renderingContext.GetTexture();
    context.rect = renderingContext.GetFrameRect();
    return context;
}

void TreeSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> TreeSprite::GetFrameTimeAndCount() {
    return {0.2f, renderingContext.GetCurrentFrameCount()};
}

void TreeSprite::SetVariant(const int newVariant) {
    renderingContext.SetVariant(newVariant);
}

int TreeSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int TreeSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

SpriteRenderingContext * TreeSprite::GetSpriteRenderingContext() { return &renderingContext; }
