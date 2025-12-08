//
// Created by Lukáš Kaplánek on 09.11.2025.
//

#include "../server/Server.h"
#include <iostream>

using namespace std;

int main() {
    Server server;
    cout << "Generating world with seed: " << server.seed << endl;
    server.generateWorld();
    cout << "Structure Map:" << endl;
    for (int x = 0; x < 512; x++) {
        for (int y = 0; y < 512; y++) {
            printf("%d ", server.worldData.collisionMap[x][y]);
        }
        printf("\n");
    }

    return 0;
}
