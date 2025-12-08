//
// Created by Lukáš Kaplánek on 05.11.2025.
//

#include "../../include/Sprites/PlayerSprite.hpp"

PlayerSprite::PlayerSprite() {
    textureName = "player";
    activeAnimation = IDLE;
    direction = DOWN;
    SpriteWidth = 96;
    SpriteHeight = 96;
    FrameSpacing = 0.0f;
    activeTexture = "player_IDLE_DOWN";
    frameCount = 8;
}
