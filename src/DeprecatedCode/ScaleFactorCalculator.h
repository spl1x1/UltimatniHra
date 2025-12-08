//
// Created by Lukáš Kaplánek on 24.10.2025.
//

#ifndef RENDERMATRIXES_H
#define RENDERMATRIXES_H


class ScaleFactorCalculator {
    float depthSensitivity; // Ovlivňuje strmost křivky změny faktoru zmenšení

    public:
    // Konstruktor s nastavením strmosti křivky
    explicit ScaleFactorCalculator(float apertureValue) : depthSensitivity(apertureValue) {}

    //změnění strmosti křivky
    void setAperture(float apertureValue){
        depthSensitivity = apertureValue;
    };

    /*
     Vrací nový faktor zmenšení na základě změny hloubky, předchozího faktoru a strmosti křivky
     Maximální hodnota 0.125f
    */
    [[nodiscard]] float CalculateNextDownscaleFactor(float depthChange, float previousFactor) const;

    static int calculateMatrixSize(float scaleFactor, int baseSize = 30);
};


#endif
