#include "render/Window.h"
#include "server/World/generace_mapy.h"
#include "server/World/Biomes/Beach/BeachBiome.h"
#include "server/World/Biomes/Biome.h"


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
    Server server = {};
    auto *c = new Window();
    c->server = server;
    c->init("Game window");
    while (true) {
        c->tick();
    }
#else

#endif
    return 0;
}
