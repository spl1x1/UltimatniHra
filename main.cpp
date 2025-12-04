#include "render/Window.h"
#include "MACROS.h"
#include <thread>


int main(int argc, char *argv[]) {
    ;

#ifdef CLIENT
    Server server = {};
    auto c = new Window();
    c->gameData.server = server;
    c->init("Game window");
    while (true) {
        c->tick();
    }
#else

#endif
    return 0;
}
