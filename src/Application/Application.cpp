//
// Created by Lukáš Kaplánek on 09.12.2025.
//

#include "../../include/Application/Application.h"

#include <iostream>

#include "../../include/Hardware/Hardware.h"


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

void Application::init() const{
    gameWindow->server = server;
    gameWindow->init(name);
}

void Application::run() const {
    try {
        while (gameWindow->data.Running) {
            gameWindow->tick();
        }
        Rml::Shutdown();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        handleException();
    }
}

Application::Application(std::string appName) : name(std::move(appName)) {};