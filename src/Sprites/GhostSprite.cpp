//
// Created by USER on 18.01.2026.
//

//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/GhostSprite.h"

void GhostSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> GhostSprite::GetFrameTimeAndCount() {
    return {renderingContext.GetFrameDuration(), renderingContext.GetCurrentFrameCount()};
}

void GhostSprite::PlayAnimation(const AnimationType newAnimation, const Direction direction, const bool ForceReset) {
    renderingContext.PlayAnimation(newAnimation, direction, ForceReset);
}

std::tuple<std::string, SDL_FRect*> GhostSprite::GetFrame() {
    std::string texture = renderingContext.GetTexture();
    return  {texture, renderingContext.GetFrameRect()};
}

int GhostSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int GhostSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

RenderingContext GhostSprite::GetRenderingContext() {
    RenderingContext context;

    const auto frame  = GetFrame();
    context.textureName = std::get<0>(frame);
    context.rect = std::get<1>(frame);

    return context;
}

SpriteRenderingContext * GhostSprite::GetSpriteRenderingContext() {
    return &renderingContext;
}

