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

void GeneraceMapy::nacist_mapu(const vector<vector<double>>& vyskaMapa, const vector<vector<double>>& vlhkostMapa, vector<vector<int>>& outbiomMapa, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, 13);

    // Generate smooth wavy borders using interpolation
    vector<double> topBorder(MAP_WIDTH);
    vector<double> bottomBorder(MAP_WIDTH);
    vector<double> leftBorder(MAP_HEIGHT);
    vector<double> rightBorder(MAP_HEIGHT);

    // Generate control points every 20-30 pixels and interpolate between them
    int controlPointSpacing = 25;
    int padding_left_right = 6;

    // Top and bottom borders
    for (int i = 0; i <= MAP_WIDTH; i += controlPointSpacing) {
        int idx = std::min(i, MAP_WIDTH - 1);
        topBorder[idx] = BORDER_SIZE + dist(rng);
        bottomBorder[idx] = BORDER_SIZE + dist(rng);
    }

    // Interpolate between control points for top and bottom
    for (int i = 0; i < MAP_WIDTH; i++) {
        int prevControl = (i / controlPointSpacing) * controlPointSpacing;
        int nextControl = std::min(prevControl + controlPointSpacing, MAP_WIDTH - 1);

        if (prevControl == nextControl) {
            topBorder[i] = topBorder[prevControl];
            bottomBorder[i] = bottomBorder[prevControl];
        } else {
            double t = static_cast<double>(i - prevControl) / (nextControl - prevControl);
            topBorder[i] = lerp(topBorder[prevControl], topBorder[nextControl], t);
            bottomBorder[i] = lerp(bottomBorder[prevControl], bottomBorder[nextControl], t);
        }
    }

    // Left and right borders
    for (int i = 0; i <= MAP_HEIGHT; i += controlPointSpacing) {
        int idx = std::min(i, MAP_HEIGHT - 1);
        leftBorder[idx] = BORDER_SIZE + dist(rng)+padding_left_right;
        rightBorder[idx] = BORDER_SIZE + dist(rng)+padding_left_right;
    }

    // Interpolate between control points for left and right
    for (int i = 0; i < MAP_HEIGHT; i++) {
        int prevControl = (i / controlPointSpacing) * controlPointSpacing;
        int nextControl = std::min(prevControl + controlPointSpacing, MAP_HEIGHT - 1);

        if (prevControl == nextControl) {
            leftBorder[i] = leftBorder[prevControl];
            rightBorder[i] = rightBorder[prevControl];
        } else {
            double t = static_cast<double>(i - prevControl) / (nextControl - prevControl);
            leftBorder[i] = lerp(leftBorder[prevControl], leftBorder[nextControl], t);
            rightBorder[i] = lerp(rightBorder[prevControl], rightBorder[nextControl], t);
        }
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Check if in border zone using the smoothed values
            bool inBorder = (x < leftBorder[y] ||
                           y < topBorder[x] ||
                           x >= MAP_WIDTH - rightBorder[y] ||
                           y >= MAP_HEIGHT - bottomBorder[x]);

            if (inBorder) {
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

    nacist_mapu(vyskaMapa, vlhkostMapa, biomMapa, mt);
}

const vector<vector<int>>& GeneraceMapy::getBiomMapa() const {
    return biomMapa;
}