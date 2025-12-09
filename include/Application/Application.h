//
// Created by Lukáš Kaplánek on 09.12.2025.
//

#ifndef ULTIMATNIHRA_APPLICATION_H
#define ULTIMATNIHRA_APPLICATION_H
#include <memory>
#include "../../include/Window/Window.h"
#include "../../include/Server/Server.h"

class Application {
    std::shared_ptr<Server> server = std::make_shared<Server>();
    std::shared_ptr<Window> gameWindow =  std::make_shared<Window>();
    std::string name;

    static void handleException();
public:
    void init() const;
    void run() const;

    explicit Application(std::string appName = "Ultimatni hra");
};


#endif //ULTIMATNIHRA_APPLICATION_H