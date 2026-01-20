//
// Created by Lukáš Kaplánek on 16.01.2026.
//

#include "../../include/Sprites/SlimeSprite_Fire.h"

void SlimeSprite_Fire::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> SlimeSprite_Fire::GetFrameTimeAndCount() {
    return {renderingContext.GetFrameDuration(), renderingContext.GetCurrentFrameCount()};
}

void SlimeSprite_Fire::PlayAnimation(const AnimationType newAnimation, const Direction direction, const bool ForceReset) {
    renderingContext.PlayAnimation(newAnimation, direction, ForceReset);
}

std::tuple<std::string, SDL_FRect> SlimeSprite_Fire::GetFrame() {
    std::string texture = renderingContext.GetTexture();
    return  {texture, renderingContext.GetFrameRect()};
}

int SlimeSprite_Fire::GetWidth() const {
    return renderingContext.GetWidth();
}

int SlimeSprite_Fire::GetHeight() const {
    return renderingContext.GetHeight();
}

RenderingContext SlimeSprite_Fire::GetRenderingContext() {
    RenderingContext context;

    const auto frame  = GetFrame();
    context.textureName = std::get<0>(frame);
    context.rect = std::get<1>(frame);

    return context;
}

SpriteRenderingContext * SlimeSprite_Fire::GetSpriteRenderingContext() {
    return &renderingContext;
}

