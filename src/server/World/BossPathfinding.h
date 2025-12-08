//
// Created by Jar Jar Banton on 1. 11. 2025.
//

#ifndef ULTIMATNIHRA_BOSSPATHFINDING_H
#define ULTIMATNIHRA_BOSSPATHFINDING_H


#pragma once
#include <vector>
#include <cmath>

struct Vec2 {
    float x, y;
    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
};

float length(const Vec2& v);
Vec2 normalize(const Vec2& v);

enum class BossState {
    IDLE,
    CHASING,
    ATTACKING,
    RETREATING
};


class Boss {
public:
    Vec2 pos;
    float movespeed = 3.0f;
    BossState state = BossState::IDLE;

    std::vector<Vec2> path;
    int pathStep = 0;

    void update(float dt, const Vec2& playerPos, const Vec2& playerVel);
    void moveToward(const Vec2& target, float dt);
    void chooseState(const Vec2& playerPos, float distance);
};

#endif //ULTIMATNIHRA_BOSSPATHFINDING_H