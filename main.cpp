#include "render/Window.h"
#include "server/Server.h"
#include "server/Event.h"

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

    Window *c = new Window("Basic window", 960, 540);

    s.Stop();
    return 0;
}
