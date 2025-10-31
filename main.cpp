#include "render/Window.h"
#include "server/World/generace_mapy.h"
#include "server/World/Biomes/Water/WaterBiome.h"
#include "server/World/Biomes/Biomes.h"


#define CLIENT


int main(int argc, char *argv[]) {;
  /*  s.onTick.add([]() {
        std::cout << "Server Tick Event" << std::endl;
    });

    s.onInitialize.add([]() {
        std::cout << "Server Initialized" << std::endl;
    });
    */

#ifdef CLIENT
    auto *c = new Window();
    c->init("Game window");
    while (true) {
        c->tick();
    }
#else

#endif
    return 0;
}
