//
// Created by Lukáš Kaplánek on 08.12.2025.
//

#ifndef ULTIMATNIHRA_DATASTRUCTURES_H
#define ULTIMATNIHRA_DATASTRUCTURES_H
#include <string>
//Defines a 2D coordinate

#include <vector>
#include <SDL3/SDL_rect.h>
#include <string>

struct Coordinates {
    float x = 0;
    float y = 0;

    Coordinates operator-(const Coordinates& other) const;
    Coordinates operator+(const Coordinates& other) const;
    Coordinates operator*(const Coordinates& other) const;
    Coordinates operator/(const Coordinates& other) const;

    Coordinates& operator+=(const Coordinates& other);
    Coordinates& operator-=(const Coordinates& other);
    Coordinates& operator/=(const Coordinates& other);
    Coordinates& operator*=(const Coordinates& other);
    Coordinates& operator+=(float value);
    Coordinates& operator-=(float value);
    Coordinates& operator/=(float value);
    Coordinates& operator*=(float value);

    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
    bool operator<(float value) const;
    bool operator>(float value) const;
};
Coordinates toTileCoordinates(const Coordinates& worldCoordinates);
Coordinates toTileCoordinates(int worldX, int worldY);
Coordinates toWorldCoordinates(const Coordinates& tileCoordinates);
Coordinates toWorldCoordinates(int tileX, int tileY);
float CoordinatesDistance(const Coordinates& a, const Coordinates& b);

struct CollisionStatus{
    bool colliding;
    bool collisionDisabled;
};

struct RenderingContext {
    std::string textureName;
    Coordinates coordinates;
    SDL_FRect rect;
};

struct HitboxContext {
    std::vector<Coordinates> corners;
    Coordinates coordinates; //Top-left coordinate for rendering
    int  r = 0;
    int  g = 0;
    int  b = 0;
    int  a = 255;
};

struct MouseData {
    float x{0.0f};
    float y{0.0f};

    float currentLeftHoldTime{0.0f};
    float currentRightHoldTime{0.0f};

    bool leftButtonPressed{false};
    bool rightButtonPressed{false};
};

enum class OreType {
    IRON,
    COPPER,
    GOLD
    // more types in future
};

struct DamageArea {
    Coordinates coordinates;
    int damage{};
    int entityId{-1}; // ID of the entity that caused the damage
};

struct PointData {
    Coordinates position;
    int r{0};
    int g{0};
    int b{0};
    int a{255};
};

struct StructureData {
    int type;
    int innerType;
    int variant;
    int inventoryId;
    int x = 0,y = 0;
};

struct EntityData {
    int type;
    int health;
    float x = 0.0f, y = 0.0f;
};

struct PlayerData {
    float health;
    float x = 0.0f, y = 0.0f;
    int inventoryId;
    int uuid{-1};
};


#endif //ULTIMATNIHRA_DATASTRUCTURES_H
