//
// Created by Jar Jar Banton on 19. 10. 2025.
//

#ifndef ULTIMATNIHRA_GENERACE_MAPY_H
#define ULTIMATNIHRA_GENERACE_MAPY_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>
#include <random>


using std::vector;
using std::cout;
using std::endl;
using std::string;

class GeneraceMapy {
public:
    GeneraceMapy(); // constructor

private:
    static constexpr int MAP_WIDTH = 100;
    static constexpr int MAP_HEIGHT = 100;

    double fade(double t);
    string ziskat_biom(double vyska, double vlhkost);
    double lerp(double a, double b, double t);
    double grad(int hash, double x, double y);
    double perlin_noise(double x, double y, vector<int>& permutace);
    void generovat_permutaci(vector<int>& permutace);
    void generovat_teren(vector<vector<double>>& mapa, vector<int>& permutace, double scale);
    void nacist_mapu(const vector<vector<double>>& vyskaMapa,const vector<vector<double>>& vlhkostMapa);
};

#endif //ULTIMATNIHRA_GENERACE_MAPY_H