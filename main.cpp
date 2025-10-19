#include "render/Window.h"
#include "server/Server.h"
#include "server/Event.h"

std::thread serverThread;


int main(int argc, char *argv[]) {;
    new std::thread([](){
        Server *s = new Server();
        s->onTick.add([](int dt){ std::cout << "tick dt=" << dt << "\n"; });


    });
    Window *c = new Window("Basic window", 960, 540);
}
