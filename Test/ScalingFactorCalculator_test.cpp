//
// Created by Lukáš Kaplánek on 24.10.2025.
//
#include <cstdio>

#include "../DeprecatedCode/ScaleFactorCalculator.h"



int main(int argc, char *argv[]) {
    ScaleFactorCalculator calculator(0.5f);
    float previousFactor = 1.0f;
    float depthChange = 0.2f;
    for (int i = 0; i < 10; ++i) {
        float newFactor = calculator.CalculateNextDownscaleFactor(depthChange, previousFactor);
        int matrixSize = ScaleFactorCalculator::calculateMatrixSize(newFactor);
        previousFactor = newFactor;
        printf("Previous Factor: %.3f, New Factor: %.3f, Matrix Size: %d\n", previousFactor, newFactor, matrixSize);
    }
    return 0;
}

