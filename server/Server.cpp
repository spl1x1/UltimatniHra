//
// Created by Lukáš Kaplánek on 17.10.2025.
//

#include "Server.h"


void Server::Tick() {
    while (isRunning) {
        onTick(tickRate);
        std::cout<<"Server Tick at rate: " << tickRate << " ticks per second." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
    }
}

void Server::Initialize() {
    isRunning = true;

}

void Server::Stop() {
    isRunning = false;

}

int Server::ChangeTickRate(int newTickRate) {
    if (newTickRate <= 0) {
        return -1;
    }
    if (tickRate==newTickRate) {
        return 0;
    }
    tickRate = newTickRate;
    sleepDuration = 1000 / tickRate;
    return 1;
}


Server::Server() {
    Initialize();
    Tick();
}

Server::~Server() {
    this->Stop();
}
