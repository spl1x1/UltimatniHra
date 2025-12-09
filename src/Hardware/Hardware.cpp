//
// Created by USER on 08.12.2025.
//

#include "../../include/Hardware/Hardware.h"

#if defined(__APPLE__) || defined(__linux__)
#include <hwinfo/hwinfo.h>

std::string Hardware::getCpu() {
    const auto cpus = hwinfo::getAllCPUs();
    return "Detected CPU: " + cpus[0].modelName() + " with " + std::to_string(cpus[0].numPhysicalCores()) + " logical processors.";
}

std::string Hardware::getGpu() {
    const auto gpus = hwinfo::getAllGPUs();
    std::string gpuName;
    std::string gpuMemory;

    return "Detected GPU: " + gpus[0].name()  + " with " + std::to_string(gpus[0].memory_Bytes() / (1024 * 1024)) + " MB VRAM.";
}

std::string Hardware::getRam() {
    const auto memory = hwinfo::Memory();
    return "Detected RAM: " + std::to_string(memory.total_Bytes() / (1024 *1024))  + " MB.";
}
#elif  WIN32
#include <windows.h>
#include <dxgi.h>

std::string Hardware::getCpu() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    HKEY hKey;
    char cpuName[256];
    DWORD bufferSize = sizeof(cpuName);

    if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)",
                     0,
                     KEY_READ,
                     &hKey) == ERROR_SUCCESS)
    return "Detected CPU: Unknown CPU with " + std::to_string(sysInfo.dwNumberOfProcessors ) + " logical processors.";

        RegQueryValueEx(hKey, "ProcessorNameString", nullptr, nullptr, (LPBYTE)cpuName, &bufferSize);
        RegCloseKey(hKey);

    return "Detected CPU: "+ std::string(cpuName) +" with " + std::to_string(sysInfo.dwNumberOfProcessors ) + " logical processors.";
}

std::string Hardware::getGpu() {
    IDXGIFactory* pFactory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
        return "Detected GPU: Failed to create DXGI factory.";

    IDXGIAdapter* pAdapter = nullptr;
    UINT i = 0;
    std::string finalGpuName;
    std::string finalGpuMemory;
    while (pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC desc;
        pAdapter->GetDesc(&desc);

        // Convert wide string to regular string
        char gpuName[128];
        wcstombs(gpuName, desc.Description, 128);

        finalGpuName+= std::string(gpuName);
        finalGpuMemory += std::to_string(desc.DedicatedVideoMemory / (1024 * 1024)) ;
        pAdapter->Release();
        break;
    }
    pFactory->Release();
    return "Detected GPU: " + finalGpuName + " with " + finalGpuMemory + " MB VRAM.";
}

std::string Hardware::getRam() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return "Detected RAM: " + std::to_string(memInfo.ullTotalPhys / (1024 * 1024))  + " MB.";
}
#else
std::string Hardware::getCpu() {
    return "CPU detection not supported on this platform.";
}
std::string Hardware::getGpu() {
    return "GPU detection not supported on this platform.";
}
std::string Hardware::getRam() {
    return "Memory detection not supported on this platform.";
}
#endif