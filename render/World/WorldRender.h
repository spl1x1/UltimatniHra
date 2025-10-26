//
// Created by USER on 21.10.2025.
//

#ifndef PREPAREWORLD_H
#define PREPAREWORLD_H

#ifndef TEXTURERES
#define TEXTURERES 32
#endif

#include "../Window.h"
#include <filesystem>



struct WorldRender {
    static void GenerateWorld(int seed,Window& window);
    static void loadSurfacesFromDirectory(const std::string& directoryPath, Window& window) ;
    static void GenerateTexture(Window& window, WorldData& worldData);

    private:
    static void ReleaseResources(Window& window);
};


#endif //PREPAREWORLD_H