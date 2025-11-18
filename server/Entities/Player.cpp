//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "Player.hpp"


Player::Player(float maxHealth, float x, float y, EntityType type, float speed, Sprite *sprite): Entity(maxHealth, x, y, type, speed, sprite) {
    cameraRect = new SDL_FRect{
        x - (GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f),
        y - (GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f),
        GAMERESW,
        GAMERESH
    };

    cameraWaterRect = new SDL_FRect{
        64,64,
        static_cast<float>(GAMERESW),
        static_cast<float>(GAMERESH) };
}


void Player::Tick(float relativeX, float relativeY)
 {

    float newX = x + relativeX;
    float newY = y + relativeY;

    if (!checkCollision(newX, newY)) return;

    x = newX;
    y = newY;

    cameraRect->x = x - cameraOffsetX;
    cameraRect->y = y - cameraOffsetY;

    cameraWaterRect->x += relativeX;
    cameraWaterRect->y += relativeY;


    if (cameraWaterRect->x > 96) cameraWaterRect->x -= 32;
    if (cameraWaterRect->x < 32) cameraWaterRect->x += 32;
    if (cameraWaterRect->y > 96) cameraWaterRect->y -= 32;
    if (cameraWaterRect->y < 32) cameraWaterRect->y += 32;
}
