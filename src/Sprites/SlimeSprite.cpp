//
// Created by Lukáš Kaplánek on 16.01.2026.
//

#include "../../include/Sprites/SlimeSprite.h"

void SlimeSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> SlimeSprite::GetFrameTimeAndCount() {
    return {renderingContext.GetFrameDuration(), renderingContext.GetCurrentFrameCount()};
}

void SlimeSprite::PlayAnimation(const AnimationType newAnimation, const Direction direction, const bool ForceReset) {
    renderingContext.PlayAnimation(newAnimation, direction, ForceReset);
}

std::tuple<std::string, SDL_FRect> SlimeSprite::GetFrame() {
    std::string texture = renderingContext.GetTexture();
    return  {texture, renderingContext.GetFrameRect()};
}

int SlimeSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int SlimeSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

RenderingContext SlimeSprite::GetRenderingContext() {
    RenderingContext context;

    const auto frame  = GetFrame();
    context.textureName = std::get<0>(frame);
    context.rect = std::get<1>(frame);

    return context;
}

SpriteRenderingContext * SlimeSprite::GetSpriteRenderingContext() {
    return &renderingContext;
}

