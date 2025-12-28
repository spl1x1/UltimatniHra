#ifndef BOSSPATHFINDINGCOORDINATEBASED_H
#define BOSSPATHFINDINGCOORDINATEBASED_H

#include <vector>
#include <cmath>

struct Vec2 {
    float x, y;
    
    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }
    
    Vec2 operator-(const Vec2& other) const {
        return {x - other.x, y - other.y};
    }
    
    Vec2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
};

enum class BossState {
    IDLE,
    CHASING,
    ATTACKING,
    RETREATING
};

namespace CoordinateBased {
    inline float length(const Vec2& v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    inline Vec2 normalize(const Vec2& v) {
        float len = length(v);
        return (len > 0.0001f) ? Vec2{v.x / len, v.y / len} : Vec2{0, 0};
    }
}

class BossCoordinateBased {
public:
    Vec2 pos;
    float movespeed = 2.0f;
    BossState state = BossState::IDLE;
    
    std::vector<Vec2> path;
    int pathStep = 0;

    void update(float dt, const Vec2& targetPos);
    void setTarget(const Vec2& newTarget);

private:
    void moveToward(const Vec2& target, float dt);
    void chooseState(float distance);
};

#endif