//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#include "ScaleFactorCalculator.h"
#include <ctgmath>


[[nodiscard]] float ScaleFactorCalculator::CalculateNextDownscaleFactor(float depthChange, float previousFactor) const {
    float factorDecrease = depthChange * depthSensitivity * 0.5f;
    float newFactor = previousFactor - factorDecrease;

    return std::max(newFactor, 0.125f);
}

int ScaleFactorCalculator::calculateMatrixSize(float scaleFactor, int baseSize) {
        return static_cast<int>(std::ceil(baseSize / scaleFactor));
}
