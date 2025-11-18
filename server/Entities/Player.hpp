//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Entity.h"
#include <SDL3/SDL.h>
#include "../../MACROS.h"



class Player final : public Entity {
    float cameraOffsetX = (static_cast<float>(GAMERESW)/ 2.0f - static_cast<float>(PLAYER_WIDTH) / 2.0f);
    float cameraOffsetY = (static_cast<float>(GAMERESH) / 2.0f -static_cast<float>(PLAYER_WIDTH)/ 2.0f);



public:
    SDL_FRect *cameraRect = nullptr;
    SDL_FRect *cameraWaterRect = nullptr;

    void Tick(float relativeX, float relativeY) override;
    Player(float maxHealth, float x, float y, EntityType type, float speed, Sprite *sprite);
};



#endif //PLAYER_HPP
