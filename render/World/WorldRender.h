//
// Created by USER on 21.10.2025.
//

#ifndef PREPAREWORLD_H
#define PREPAREWORLD_H
#include "../Window.h"


struct WorldRender {
    static void loadTexturesFromDirectory(const std::string& directoryPath, Window& window) ;
    static void GenerateTexture(Window& window);
};


#endif //PREPAREWORLD_H