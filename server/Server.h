//
// Created by Lukáš Kaplánek on 17.10.2025.
//

#ifndef SERVER_H
#define SERVER_H

#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include "Event.h"

using namespace nlohmann::literals;

class Server {
    //Private Members
    bool isRunning = false;
    int tickRate = 20; // Server ticks per second
    int sleepDuration = 1000 / tickRate;

    //Private Methods
    void Tick();
public:


    //Events
    Event<void()> onTick;
    Event<void()> onInitialize;

    void Initialize();
    int ChangeTickRate(int newTickRate);
    void Stop();
    ~Server();
};



#endif //SERVER_H
