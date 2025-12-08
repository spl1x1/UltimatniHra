//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#include "Structure.h"
#include "../../render/Sprites/Sprite.hpp"

Structure::Structure(int id, structureType type, std::unique_ptr<Sprite> sprite) {
    this->id = id;
    this->type = type;
    this->sprite = std::move(sprite);
}