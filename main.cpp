#include "render/Window.h"
#include "MACROS.h"
#include <thread>
#include <hwinfo/battery.h>
#include <hwinfo/cpu.h>
#include <hwinfo/gpu.h>
#include <hwinfo/ram.h>

int main(int argc, char *argv[]) {
#ifdef CLIENT

    Server server = {};
    auto c = new Window();
    c->gameData.server = server;
    c->init("Game window");

    try {
        while (c->data.inited) {
            c->tick();
        }
    }
    catch (...) {
        delete c;
        std::cerr << "-----------------------------------------------"<< std::endl;
        std::cerr << "Exception: Your computer is not powerful enough!"<< std::endl;
        const auto cpus = hwinfo::getAllCPUs();
        const auto gpus = hwinfo::getAllGPUs();
        const auto memory = hwinfo::Memory();
        for (const auto& cpu : cpus) {
            std::cerr << "Detected CPU: " << cpu.modelName() << " with " << cpu.numPhysicalCores() << " cores." << std::endl;
        }
        for (const auto& gpu : gpus) {
            std::cerr << "Detected GPU: " << gpu.name() << " with " << gpu.memory_Bytes() << " MB VRAM." << std::endl;
        }
        std::cerr << "Detected RAM: " << memory.total_Bytes() / (1024 *1024) << " MB." << std::endl;
        std::cerr << "Recommended minimum specs: CPU with 64 cores, memory size of at least 131072 MB and GPU with 20480MB VRAM." << std::endl;
        std::cerr << "Multi GPU setup recommended!" << std::endl;
    }
#endif
    exit(0);
}
