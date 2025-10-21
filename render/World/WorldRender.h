//
// Created by USER on 21.10.2025.
//

#ifndef PREPAREWORLD_H
#define PREPAREWORLD_H
#include "../Window.h"


struct WorldRender {
    static void GenerateTexture(Window& window, std::pmr::vector<std::pmr::vector<int>>& worldMap);
};


#endif //PREPAREWORLD_H
