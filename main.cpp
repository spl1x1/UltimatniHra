#include "render/Window.h"
#include "server/World/generace_mapy.h"



int main(int argc, char *argv[]) {;
  /*  s.onTick.add([]() {
        std::cout << "Server Tick Event" << std::endl;
    });

    s.onInitialize.add([]() {
        std::cout << "Server Initialized" << std::endl;
    });
    */

    auto *c = new Window();
    c->init("Game window");
    return 0;
}
