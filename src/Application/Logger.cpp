//
// Created by USER on 15.12.2025.
//
#include "../../include/Application/Logger.h"

#include <iomanip>

#include "../../include/Application/MACROS.h"

#include <SDL3/SDL_log.h>

std::unique_ptr<Logger> Logger::loggerInstance = nullptr;
std::mutex Logger::logMutex;

Logger::Logger() {
    running = true;
    logThread = std::thread(&Logger::logWorker, this);
    #ifdef LOGGER_LOG_TO_FILE
    logFile = std::ofstream("application.log", std::ios::app);
    logFile << "----- Logger started -----" << std::endl;
    #endif
}

void Logger::InternalLog(const std::string &message) {
    #ifdef LOGGER_LOG_TO_FILE
    logFile << message << std::endl;
    #endif
    #ifdef LOGGER_LOG_SDL
    SDL_Log("%s", message.c_str());
    #endif
};

void Logger::logWorker() {
    auto now = std::chrono::system_clock::now();
    while (running) {
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::string message;
        {
            std::lock_guard lock(logMutex);
            if (!logQueue.empty()) {
                message += logQueue.front();
                logQueue.pop();
            }
        }
        std::stringstream ss;
        ss << "["<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X] ");

        message.empty() ? std::this_thread::sleep_for(std::chrono::milliseconds(100)) :  InternalLog( ss.str() + message);
        }
}


void Logger::Log(const std::string &message) {
    std::lock_guard lock(logMutex);
    if (loggerInstance) loggerInstance->logQueue.push(message);
}

void Logger::Init() {
    if (loggerInstance) return;
#ifdef LOGGER_ENABLED
    loggerInstance = std::unique_ptr<Logger>(new Logger());
#endif
}

void Logger::Shutdown() {
    if (!loggerInstance) return;
    {
        std::lock_guard lock(logMutex);
        loggerInstance->running = false;
    }
    loggerInstance->logThread.join();
    #ifdef LOGGER_LOG_TO_FILE
    if (loggerInstance->logFile.is_open()) {
        loggerInstance->logFile.close();
    }
    #endif
    loggerInstance.reset();
}
