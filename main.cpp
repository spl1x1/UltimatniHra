#include "render/Window.h"
#include "MACROS.h"

#ifdef UNIX
#include <hwinfo/hwinfo.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#endif


int main(int argc, char *argv[]) {
#ifdef CLIENT

    Server server = {};
    auto c = new Window();
    c->server = &server;
    c->init("Game window");

    try {
        while (c->data.inited) {
            c->tick();
        }
    }
    catch (...) {
        std::cerr << "-----------------------------------------------"<< std::endl;
        std::cerr << "Exception: Your computer is not powerful enough!"<< std::endl;

#ifdef UNIX
        const auto cpus = hwinfo::getAllCPUs();
        const auto gpus = hwinfo::getAllGPUs();
        const auto memory = hwinfo::Memory();
        for (const auto& cpu : cpus) {
            std::cerr << "Detected CPU: " << cpu.modelName() << " with " << cpu.numPhysicalCores() << " cores." << std::endl;
        }
        for (const auto& gpu : gpus) {
            std::cerr << "Detected GPU: " << gpu.name() << " with " << gpu.memory_Bytes()/1024/1024 << " MB VRAM." << std::endl;
        }
        std::cerr << "Detected RAM: " << memory.total_Bytes() / (1024 *1024) << " MB." << std::endl;

#endif
#ifdef _WIN32
        // CPU Detection
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        std::cerr << "Detected CPU: " << sysInfo.dwNumberOfProcessors << " logical processors." << std::endl;

        // RAM Detection
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        std::cerr << "Detected RAM: " << memInfo.ullTotalPhys / (1024 * 1024) << " MB." << std::endl;

        // GPU Detection using DXGI
        IDXGIFactory* pFactory = nullptr;
        if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
            IDXGIAdapter* pAdapter = nullptr;
            UINT i = 0;
            while (pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
                DXGI_ADAPTER_DESC desc;
                pAdapter->GetDesc(&desc);

                // Convert wide string to regular string
                char gpuName[128];
                wcstombs(gpuName, desc.Description, 128);

                std::cerr << "Detected GPU: " << gpuName << " with "
                          << desc.DedicatedVideoMemory / (1024 * 1024) << " MB VRAM." << std::endl;

                pAdapter->Release();
                i++;
            }
            pFactory->Release();
        }
#endif
        std::cerr << "Recommended minimum specs: CPU with 64 cores, memory size of at least 131072 MB and GPU with 20480MB VRAM." << std::endl;
        std::cerr << "Multi GPU setup recommended!" << std::endl;
    }
#endif
    delete c;
    exit(0);
}
