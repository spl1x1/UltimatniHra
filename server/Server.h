//
// Created by Lukáš Kaplánek on 17.10.2025.
//

#ifndef SERVER_H
#define SERVER_H

#include <nlohmann/json.hpp>

using namespace nlohmann::literals;

class Server {
    int port;
    bool isRunning;

public:
    Server();

#ifdef __fromPython__

#endif
};



#endif //SERVER_H
