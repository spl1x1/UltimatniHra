//
// Created by Jar Jar Banton on 7. 12. 2025.
//
#include "../../include/Server/BossPathfindingCoordinateBased.h"
#include <iostream>

using namespace CoordinateBased;

void BossCoordinateBased::update(float dt, const Vec2& targetPos) {
    float dist = length(targetPos - pos);
    chooseState(dist);

    switch (state) {
        case BossState::IDLE:
            break;

        case BossState::CHASING:
            if (path.empty() || pathStep >= (int)path.size()) {
                moveToward(targetPos, dt);
            } else {
                moveToward(path[pathStep], dt);
                if (length(path[pathStep] - pos) < 0.2f)
                    pathStep++;
            }
            break;

        case BossState::ATTACKING:
            std::cout << "Boss attacks!" << std::endl;
            moveToward(targetPos, dt * 0.3f);
            break;

        case BossState::RETREATING: {
            Vec2 awayDir = normalize(pos - targetPos);
            Vec2 retreatPoint = pos + awayDir * 5.0f;
            moveToward(retreatPoint, dt);
            break;
        }
    }
}

void BossCoordinateBased::moveToward(const Vec2& target, float dt) {
    Vec2 dir = normalize(target - pos);
    pos = pos + dir * movespeed * dt;
}

void BossCoordinateBased::chooseState(float distance) {
    if (distance < 1.5f)
        state = BossState::ATTACKING;
    else if (distance < 10.0f)
        state = BossState::CHASING;
    else
        state = BossState::IDLE;
}

void BossCoordinateBased::setTarget(const Vec2& newTarget) {
    path.clear();
    pathStep = 0;
}