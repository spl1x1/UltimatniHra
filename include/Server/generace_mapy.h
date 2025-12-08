//
// Created by Jar Jar Banton on 19. 10. 2025.
//

#ifndef ULTIMATNIHRA_GENERACE_MAPY_H
#define ULTIMATNIHRA_GENERACE_MAPY_H

#include <iostream>
#include <vector>
#include <random>

using std::vector;
using std::cout;
using std::endl;
using std::string;

class GeneraceMapy
{
public:
    GeneraceMapy(unsigned int seed); // constructor

    vector<vector<int>> biomMapa;
    static constexpr int BORDER_SIZE = 7; // minimum border size
    const vector<vector<int>>& getBiomMapa() const;

private:
    // Biome identifiers as enum
    enum Biome {
        VODA = 0,
        PLAZ = 1,
        POUST = 2,
        TRAVA = 3,
        SKALA = 4,
        LES = 5,
        SNIH = 6
    };

    double fade(double t);
    int ziskat_biom(double vyska, double vlhkost);
    double lerp(double a, double b, double t);
    double grad(int hash, double x, double y);
    double perlin_noise(double x, double y, vector<int>& permutace);
    void generovat_permutaci(vector<int>& permutace, std::mt19937& rng);
    void generovat_teren(vector<vector<double>>& mapa, vector<int>& permutace, double scale);
    void nacist_mapu(const vector<vector<double>>& vyskaMapa, const vector<vector<double>>& vlhkostMapa, vector<vector<int>>& biomMapa, std::mt19937& rng);
};

#endif //ULTIMATNIHRA_GENERACE_MAPY_H