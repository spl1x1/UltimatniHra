//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "Player.hpp"


Player::Player(float maxHealth, Coordinates coordinates, EntityType type, float speed, Sprite *sprite): Entity(maxHealth,coordinates, type, speed, sprite) {
    cameraRect = new SDL_FRect{
        coordinates.x - (GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f),
        coordinates.y - (GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f),
        GAMERESW,
        GAMERESH
    };

    cameraWaterRect = new SDL_FRect{
        64,64,
        static_cast<float>(GAMERESW),
        static_cast<float>(GAMERESH)
    };

    Hitbox playerHitbox = {
        {
            {32, 32}, // TOP_LEFT
            {64 ,32}, // TOP_RIGHT
            {64, 65}, // BOTTOM_RIGHT
            {32,65} // BOTTOM_LEFT
        }
    };
        SetHitbox(playerHitbox);
    setSpriteOffsetX(47);
    setSpriteOffsetY(47);
};


bool Player::Tick(float relativeX, float relativeY)
 {
    if (!Entity::Tick(relativeX, relativeY)) return false;

    cameraRect->x = coordinates.x - cameraOffsetX;
    cameraRect->y = coordinates.y - cameraOffsetY;

    cameraWaterRect->x += relativeX;
    cameraWaterRect->y += relativeY;

    if (cameraWaterRect->x > 96) cameraWaterRect->x -= 32;
    if (cameraWaterRect->x < 32) cameraWaterRect->x += 32;
    if (cameraWaterRect->y > 96) cameraWaterRect->y -= 32;
    if (cameraWaterRect->y < 32) cameraWaterRect->y += 32;
    return true;
}
