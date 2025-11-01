#include "render/Window.h"
#include "server/World/generace_mapy.h"
#include "server/World/Biomes/Water/WaterBiome.h"
#include "server/World/Biomes/Biome.h"


#define CLIENT

void RegisterBiomes(Server &server) {
    server.biomes.push_back(new WaterBiome());
}




int main(int argc, char *argv[]) {;
  /*  s.onTick.add([]() {
        std::cout << "Server Tick Event" << std::endl;
    });

    s.onInitialize.add([]() {
        std::cout << "Server Initialized" << std::endl;
    });
    */

#ifdef CLIENT
    Server server;
    RegisterBiomes(server);
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
