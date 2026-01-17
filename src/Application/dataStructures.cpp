//
// Created by Lukáš Kaplánek on 17.01.2026.
//

#include "../../include/Application/dataStructures.h"

Coordinates Coordinates::operator-(const Coordinates &other) const {
    return {x-other.x, y - other.y};
}

Coordinates Coordinates::operator+(const Coordinates &other) const {
    return {x+other.x, y+other.y};
}

bool Coordinates::operator==(const Coordinates &other) const {
    return x == other.x && y == other.y;
}

bool Coordinates::operator!=(const Coordinates &other) const {
    return !(*this == other);
}

bool Coordinates::operator<(const float value) const {
    return x < value && y < value;
}

bool Coordinates::operator>(const float value) const {
    return x > value && y > value;
}

Coordinates& Coordinates::operator*=(const Coordinates &other) {
    return *this = *this * other;
}

Coordinates Coordinates::operator*(const Coordinates &other) const {
    return {x * other.x, y * other.y};
}

Coordinates Coordinates::operator/(const Coordinates &other) const {
    return {x / other.x, y / other.y};
}

Coordinates& Coordinates::operator+=(const Coordinates &other) {
    return *this = *this + other;
}

Coordinates& Coordinates::operator-=(const Coordinates &other) {
    return *this = *this - other;
}

Coordinates& Coordinates::operator/=(const Coordinates &other) {
    return *this = *this / other;
}

Coordinates& Coordinates::operator+=(float value) {
    return *this = *this + Coordinates{value, value};
}

Coordinates& Coordinates::operator-=(float value) {
    return *this = *this - Coordinates{value, value};
}

Coordinates& Coordinates::operator/=(float value) {
    return *this = Coordinates{x / value, y / value};
}

Coordinates& Coordinates::operator*=(float value) {
    return *this = Coordinates{x * value, y * value};
}

Coordinates toTileCoordinates(const Coordinates &worldCoordinates) {
    return {
        std::floor(worldCoordinates.x / 32.0f),
        std::floor(worldCoordinates.y / 32.0f)
    };
}

Coordinates toTileCoordinates(int worldX, int worldY) {
    return {
        std::floor(static_cast<float>(worldX) / 32.0f),
        std::floor(static_cast<float>(worldY) / 32.0f)
    };
}

Coordinates toWorldCoordinates(const Coordinates &tileCoordinates) {
    return {
        tileCoordinates.x * 32.0f,
        tileCoordinates.y * 32.0f
    };
}

Coordinates toWorldCoordinates(int tileX, int tileY) {
    return {
        static_cast<float>(tileX) * 32.0f,
        static_cast<float>(tileY) * 32.0f
    };}

float CoordinatesDistance(const Coordinates &a, const Coordinates &b) {
    const float dx{a.x - b.x};
    const float dy{a.y - b.y};
    return std::sqrt(dx * dx + dy * dy);
}

