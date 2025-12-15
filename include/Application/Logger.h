//
// Created by USER on 15.12.2025.
//

#ifndef LOGGER_H
#define LOGGER_H

#include "MACROS.h"
#include <thread>
#include <queue>
#include <fstream>


class Logger {
    std::ofstream logFile;
    std::thread logThread;
    std::queue<std::string> logQueue;
    void logWorker();
    bool running{false};

    static std::mutex logMutex;
    static std::unique_ptr<Logger> loggerInstance;
    Logger();

    void InternalLog(const std::string &message);

public:
    static void Log(const std::string &message);
    static void Init();
    static void Shutdown();
};
#endif //LOGGER_H
