//
// Created by Jar Jar Banton on 1. 11. 2025.
//

#include "BossPathfinding.h"
#include <iostream>
#include <algorithm>

float length(const Vec2& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vec2 normalize(const Vec2& v) {
    float len = length(v);
    return (len > 0.0001f) ? Vec2{v.x / len, v.y / len} : Vec2{0, 0};
}

void Boss::update(float dt, const Vec2& playerPos, const Vec2& playerVel) {

    Vec2 predictedPlayer = playerPos + playerVel * 0.5f;

    float dist = length(predictedPlayer - pos);
    chooseState(playerPos, dist);

    switch (state) {
        case BossState::IDLE:
            break;

        case BossState::CHASING:


            if (path.empty() || pathStep >= (int)path.size()) {
                moveToward(predictedPlayer, dt);
            } else {
                moveToward(path[pathStep], dt);
                if (length(path[pathStep] - pos) < 0.2f)
                    pathStep++;
            }
            break;

        case BossState::ATTACKING:
            std::cout << "Boss attacks!" << std::endl;
            break;

        case BossState::RETREATING:
            moveToward({pos.x - 1.0f, pos.y - 1.0f}, dt);
            break;
    }
}

void Boss::moveToward(const Vec2& target, float dt) {
    Vec2 dir = normalize(target - pos);
    pos = pos + dir * movespeed * dt;
}

void Boss::chooseState(const Vec2& playerPos, float distance) {
    if (distance < 1.5f)
        state = BossState::ATTACKING;
    else if (distance < 10.0f)
        state = BossState::CHASING;
    else
        state = BossState::IDLE;
}