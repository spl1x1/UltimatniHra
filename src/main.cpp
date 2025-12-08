#include "../include/Window/Window.h"
#include "../include/MACROS.h"
#include "../include/Server/Server.h"
#include "../include/Hardware/Hardware.h"


int main(int argc, char *argv[]) {
#ifdef CLIENT

    auto server = std::make_shared<Server>();
    auto gameWindow =  std::make_shared<Window>();
    gameWindow->server = std::move(server);
    gameWindow->init("Game window");

    try {
        while (gameWindow->data.inited) {
            gameWindow->tick();
        }
    }
    catch (...) {
        std::cerr << "-----------------------------------------------"<< std::endl;
        std::cerr << "Exception: Your computer is not powerful enough!"<< std::endl;
        std::cerr << Hardware::getCpu() << std::endl;
        std::cerr << Hardware::getRam() << std::endl;
        std::cerr << Hardware::getGpu() << std::endl;
        std::cerr << "-----------------------------------------------"<< std::endl;
        std::cerr << "Recommended minimum specs: CPU with 64 cores, memory size of at least 131072 MB and GPU with 20480MB VRAM." << std::endl;
        std::cerr << "Multi GPU setup recommended!" << std::endl;
    }
#endif
    return 0;
}
