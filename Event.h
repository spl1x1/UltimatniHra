//
// Created by USER on 04.12.2025.
//

#ifndef EVENT_H
#define EVENT_H
#include <functional>
#include <mutex>
#include <string>


struct EventCallback {
    std::function<void()> func;
    std::string callbackName;
};

class Event {
    std::vector<EventCallback> registeredEvents;
    std::mutex eventMutex;
public:

    void registerEvent(const EventCallback& EventCallback) {
        eventMutex.lock();
        registeredEvents.push_back(EventCallback);
        eventMutex.unlock();
    }

    void unregisterEvent(const std::string& EventName) {
        eventMutex.lock();
        for (auto &event : registeredEvents) {
            if (event.callbackName == EventName) {
                std::erase(registeredEvents, event);
                break;
            }
        }
        eventMutex.unlock();
    }

    void triggerEvents() {
        eventMutex.lock();
        for (const auto& event : registeredEvents) {
            event.func();
        }
        eventMutex.unlock();
    }

    void operator()() {
        triggerEvents();
    }

    void operator +=(const EventCallback& EventCallback) {
        registerEvent(EventCallback);
    }
};



#endif //EVENT_H
