#define MAP_WIDTH 512
#define MAP_HEIGHT 512


#include "render/Window.h"
#include "server/Server.h"
#include "server/Event.h"
#include "server/World/generace_mapy.h"


std::thread serverThread;
Server s;

int main(int argc, char *argv[]) {;
  /*  s.onTick.add([]() {
        std::cout << "Server Tick Event" << std::endl;
    });

    s.onInitialize.add([]() {
        std::cout << "Server Initialized" << std::endl;
    });
    */



    new std::thread([]() {
        s.Initialize();
    });

    GeneraceMapy generaceMapy = GeneraceMapy();
    auto *c = new Window();
    c->worldData.WorldMap = generaceMapy.biomMapa;
    c->init("Game window");

    s.Stop();
    return 0;
}
