//
// Created by Lukáš Kaplánek on 17.10.2025.
//

#include "Server.h"


void Server::Tick() {
    while (isRunning) {
        onTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
    }
}

void Server::Initialize() {
    onInitialize();
    if (isRunning) {
        return;
    }
    isRunning = true;
    Tick();
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

Server::~Server() {
    this->Stop();
}
