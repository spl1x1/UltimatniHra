#include "generace_mapy.h"


#define MAP_WIDTH 100
#define MAP_HEIGHT 100

double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10); //perlin funkce
}
double lerp(double a, double b, double t) {
    return a + t * (b - a);
}
double grad(int hash, double x, double y) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return (h & 1 ? -u : u) + (h & 2 ? -v : v);
}

double perlin_noise(double x, double y, vector<int>& permutace) {
    //vypocet coordsky:
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    //coordsky v gridu:
    double xf = x - floor(x);
    double yf = y - floor(y);

    double u = fade(xf);
    double v = fade(yf);
    //hash coordsek podle bodu v gridu:
    int a = permutace[X] + Y;
    int aa = permutace[a];
    int ab = permutace[a + 1];
    int b = permutace[X + 1] + Y;
    int ba = permutace[b];
    int bb = permutace[b + 1];
    //interpolace:
    double gradAA = grad(permutace[aa], xf, yf);
    double gradAB = grad(permutace[ab], xf, yf - 1);
    double gradBA = grad(permutace[ba], xf - 1, yf);
    double gradBB = grad(permutace[bb], xf - 1, yf - 1);

    double x1 = ::lerp(gradAA, gradBA, u); //pozor problem s ::lerp je to lehce retardovane
    double x2 = ::lerp(gradAB, gradBB, u);

    return ::lerp(x1, x2, v);
}
void generovat_permutaci(vector<int>& permutace) {
    for (int i = 0; i < 256; i++) {
        permutace[i] = i;
    }
    random_shuffle(permutace.begin(), permutace.end());
    permutace.insert(permutace.end(), permutace.begin(), permutace.end());
}

void generovat_teren(vector<vector<double>>&mapa, vector<int>& permutace, double scale) {
    for (int x = 0;x<MAP_WIDTH;x++) {
        for (int y = 0;y<MAP_HEIGHT;y++) {
            double nx = x / scale;
            double ny = y / scale;

            mapa[x][y] = perlin_noise(nx, ny, permutace);
        }
    }
}
void nacist_mapu(const vector<vector<double>>& mapa) {
    for (int x = 0;x<MAP_WIDTH;x++) {
        for (int y = 0;y<MAP_HEIGHT;y++) {
            if (mapa[x][y] < -0.5) {
                cout << "voda"; //voda
            }
            else if (mapa[x][y] < 0.5) {
                cout << "trava"; //trava
            }
            else {
                cout << "hora"; //hora
            }
        }
        cout << endl;
    }
}

GeneraceMapy::GeneraceMapy() {
    srand(static_cast<unsigned int>(time(0))); //random seed

    vector<int> permutace(256);
    generovat_permutaci(permutace);

    vector<vector<double>> mapa(MAP_WIDTH, vector<double>(MAP_HEIGHT));
    double scale = 20.0; //zvetseni/mensi skala
    generovat_teren(mapa, permutace, scale);
    nacist_mapu(mapa);
    return 0;
}



//
// Created by Jar Jar Banton on 18. 10. 2025.
//