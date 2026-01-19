//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/PlayerSprite.hpp"

void PlayerSprite::Tick(const float deltaTime) {
    renderingContext.Tick(deltaTime);
}

std::tuple<float, int> PlayerSprite::GetFrameTimeAndCount() {
    return {renderingContext.GetFrameDuration(), renderingContext.GetCurrentFrameCount()};
}

void PlayerSprite::PlayAnimation(const AnimationType newAnimation, const Direction direction, const bool ForceReset) {
    renderingContext.PlayAnimation(newAnimation, direction, ForceReset);
}

std::tuple<std::string, SDL_FRect> PlayerSprite::GetFrame() {
    std::string texture = renderingContext.GetTexture();
    return  {texture, renderingContext.GetFrameRect()};
}

int PlayerSprite::GetWidth() const {
    return renderingContext.GetWidth();
}

int PlayerSprite::GetHeight() const {
    return renderingContext.GetHeight();
}

RenderingContext PlayerSprite::GetRenderingContext() {
    RenderingContext context;

    const auto frame  = GetFrame();
    context.textureName = std::get<0>(frame);
    context.rect = std::get<1>(frame);

    return context;
}

SpriteRenderingContext * PlayerSprite::GetSpriteRenderingContext() {
    return &renderingContext;
}

