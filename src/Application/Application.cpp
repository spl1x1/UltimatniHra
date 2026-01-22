//
// Created by Lukáš Kaplánek on 09.12.2025.
//

#include "../../include/Application/Application.h"

#include <iostream>

#include "../../include/Application/SaveGame.h"
#include "../../include/Hardware/Hardware.h"

// Define the static member declared in the header
Application* Application::instance = nullptr;

void Application::handleException() {
    std::cerr << "-----------------------------------------------"<< std::endl;
    std::cerr << "Exception: Your computer is not powerful enough!"<< std::endl;
    std::cerr << Hardware::getCpu() << std::endl;
    std::cerr << Hardware::getRam() << std::endl;
    std::cerr << Hardware::getGpu() << std::endl;
    std::cerr << "-----------------------------------------------"<< std::endl;
    std::cerr << "Recommended minimum specs: CPU with 64 cores, memory size of at least 131072 MB and GPU with 20480MB VRAM." << std::endl;
    std::cerr << "Multi GPU setup recommended!" << std::endl;
}

void Application::SignalHandler(const int sig) {
    SDL_Log("Signal %d received, shutting down...", sig);
    instance->server->SaveServerState();
    auto &saveManager{SaveManager::getInstance()};
    saveManager.saveGame(saveManager.getCurrentSlot(), instance->server.get());
    Rml::Shutdown();
    exit(0);
}

void Application::init() {
    if (instance) return;
    instance = this;
    gameWindow->server = server;
    gameWindow->init(name);

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGABRT, SignalHandler);
    signal(SIGSEGV, SignalHandler);
}

void Application::run() const {
    //while (gameWindow->data.Running) gameWindow->tick();

    try {
        while (gameWindow->data.Running) gameWindow->tick();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        handleException();
    }
}

Application* Application::GetInstance() {
    return instance;
}

Application::Application(std::string appName) : name(std::move(appName)) {
}
