#include "render/Window.h"
#include "server/Event.h"
#include "server/World/generace_mapy.h"



int main(int argc, char *argv[]) {;
  /*  s.onTick.add([]() {
        std::cout << "Server Tick Event" << std::endl;
    });

    s.onInitialize.add([]() {
        std::cout << "Server Initialized" << std::endl;
    });
    */

    GeneraceMapy generaceMapy = GeneraceMapy();
    Window *c = new Window();
    c->WorldMap = generaceMapy.biomMapa;
    c->init("Game window");
    return 0;
}
