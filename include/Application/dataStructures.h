//
// Created by Lukáš Kaplánek on 08.12.2025.
//

#ifndef ULTIMATNIHRA_DATASTRUCTURES_H
#define ULTIMATNIHRA_DATASTRUCTURES_H
#include <vector>
#include <SDL3/SDL_rect.h>

//Defines a 2D coordinate
struct Coordinates {
    float x = 0;
    float y = 0;
};

struct TileCoordinates {
    int x = 0;
    int y = 0;
};

struct CollisionStatus{
    bool colliding;
    bool collisionDisabled;
};

struct RenderingContext {
    std::string textureName;
    Coordinates coordinates;
    SDL_FRect* rect = nullptr;
};

struct HitboxContext {
    std::vector<Coordinates> corners;
    Coordinates coordinates; //Top-left coordinate for rendering
    int  r = 0;
    int  g = 0;
    int  b = 0;
    int  a = 255;
};

struct MousePosition {
    float x{0.0f};
    float y{0.0f};
};

enum class OreType {
    IRON,
    COPPER,
    GOLD
    // more types in future
};

struct DamageArea {
    TileCoordinates tile;
    int damage{};
    int entityId{-1}; // ID of the entity that caused the damage
    int damageInstatnceId{-1}; // Unique ID for this damage instance
};


#endif //ULTIMATNIHRA_DATASTRUCTURES_H
