//
// Created by Jar Jar Banton on 18. 10. 2025.
//

#ifndef MAP_WIDTH
#define MAP_WIDTH 512
#endif
#ifndef MAP_HEIGHT
#define MAP_HEIGHT 512
#endif

#include "generace_mapy.h"

double GeneraceMapy::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double GeneraceMapy::lerp(double a, double b, double t) {
    return a + t * (b - a);
}

double GeneraceMapy::grad(int hash, double x, double y) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return (h & 1 ? -u : u) + (h & 2 ? -v : v);
}

double GeneraceMapy::perlin_noise(double x, double y, vector<int>& permutace) {
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    double xf = x - floor(x);
    double yf = y - floor(y);

    double u = fade(xf);
    double v = fade(yf);

    int a = permutace[X] + Y;
    int aa = permutace[a];
    int ab = permutace[a + 1];
    int b = permutace[X + 1] + Y;
    int ba = permutace[b];
    int bb = permutace[b + 1];

    double gradAA = grad(permutace[aa], xf, yf);
    double gradAB = grad(permutace[ab], xf, yf - 1);
    double gradBA = grad(permutace[ba], xf - 1, yf);
    double gradBB = grad(permutace[bb], xf - 1, yf - 1);

    double x1 = lerp(gradAA, gradBA, u);
    double x2 = lerp(gradAB, gradBB, u);

    return lerp(x1, x2, v);
}

void GeneraceMapy::generovat_permutaci(vector<int>& permutace, std::mt19937& rng) {
    for (int i = 0; i < 256; i++) {
        permutace[i] = i;
    }
    std::shuffle(permutace.begin(), permutace.begin() + 256, rng);
    permutace.insert(permutace.end(), permutace.begin(), permutace.begin() + 256);
}

void GeneraceMapy::generovat_teren(vector<vector<double>>& mapa, vector<int>& permutace, double scale) {
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            double nx = x / scale;
            double ny = y / scale;
            mapa[x][y] = perlin_noise(nx, ny, permutace);
        }
    }
}

void GeneraceMapy::nacist_mapu(const vector<vector<double>>& vyskaMapa, const vector<vector<double>>& vlhkostMapa, vector<vector<int>>& outbiomMapa, std::mt19937& rng, std::uniform_real_distribution<double>& dist) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int random_border = static_cast<int>(dist(rng)) + BORDER_SIZE;
            if (x < random_border || y < random_border || x >= MAP_WIDTH - random_border || y >= MAP_HEIGHT - random_border) {
                outbiomMapa[x][y] = VODA;
                continue;
            }
            double vyska = (vyskaMapa[x][y] + 1.0) / 2.0;
            double vlhkost = (vlhkostMapa[x][y] + 1.0) / 2.0;
            outbiomMapa[x][y] = ziskat_biom(vyska, vlhkost);
        }
    }
}

int GeneraceMapy::ziskat_biom(double vyska, double vlhkost) {
    if (vyska < 0.3) {
        return VODA;
    } else if (vyska < 0.4) {
        return PLAZ;
    } else if (vyska < 0.6) {
        return (vlhkost < 0.5) ? POUST : TRAVA;
    } else if (vyska < 0.8) {
        return (vlhkost < 0.4) ? SKALA : LES;
    } else {
        return SNIH;
    }
}

GeneraceMapy::GeneraceMapy(unsigned int seed) {
    std::srand(static_cast<unsigned int>(seed));
    std::mt19937 mt(seed);
    std::uniform_real_distribution<double> dist(1.0, VARIATION_LEVELS);

    vector<int> permutace1(512);
    vector<int> permutace2(512);
    generovat_permutaci(permutace1, mt);
    generovat_permutaci(permutace2, mt);

    vector<vector<double>> vyskaMapa(MAP_WIDTH, vector<double>(MAP_HEIGHT));
    vector<vector<double>> vlhkostMapa(MAP_WIDTH, vector<double>(MAP_HEIGHT));

    biomMapa.resize(MAP_WIDTH, vector<int>(MAP_HEIGHT));

    double scale = 150.0;
    generovat_teren(vyskaMapa, permutace1, scale);
    generovat_teren(vlhkostMapa, permutace2, scale);

    nacist_mapu(vyskaMapa, vlhkostMapa, biomMapa, mt, dist);
}

const vector<vector<int>>& GeneraceMapy::getBiomMapa() const {
    return biomMapa;
}