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

void GeneraceMapy::generovat_permutaci(vector<int>& permutace) {
    for (int i = 0; i < 256; i++) {
        permutace[i] = i;
    }
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::shuffle(permutace.begin(), permutace.end(), rng);
    permutace.insert(permutace.end(), permutace.begin(), permutace.end());

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

void GeneraceMapy::nacist_mapu(const vector<vector<double>>& vyskaMapa,const vector<vector<double>>& vlhkostMapa) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            double vyska = (vyskaMapa[x][y] + 1.0) / 2.0;      // Normalize to [0, 1]
            double vlhkost = (vlhkostMapa[x][y] + 1.0) / 2.0;  // Normalize to [0, 1]
            std::string biom = ziskat_biom(vyska, vlhkost);
            cout << biom << " ";
        }
        cout << endl;
    }
}
std::string GeneraceMapy::ziskat_biom(double vyska, double vlhkost) {
    if (vyska < 0.3) {
        return "voda";
    } else if (vyska < 0.4) {
        return "plaz";
    } else if (vyska < 0.6) {
        if (vlhkost < 0.5) return "poust";
        else return "trava";
    } else if (vyska < 0.8) {
        if (vlhkost < 0.4) return "skala";
        else return "les";
    } else {
        return "snih";
    }
}

GeneraceMapy::GeneraceMapy() {
    vector<int> permutace1(256);
    vector<int> permutace2(256);
    generovat_permutaci(permutace1);
    generovat_permutaci(permutace2);

    vector<vector<double>> vyskaMapa(MAP_WIDTH, vector<double>(MAP_HEIGHT));
    vector<vector<double>> vlhkostMapa(MAP_WIDTH, vector<double>(MAP_HEIGHT));

    double scale = 20.0;
    generovat_teren(vyskaMapa, permutace1, scale);
    generovat_teren(vlhkostMapa, permutace2, scale); // 2nd layer

    nacist_mapu(vyskaMapa, vlhkostMapa);
}



//
// Created by Jar Jar Banton on 18. 10. 2025.
//