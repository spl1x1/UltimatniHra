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

class EntityEvent {
    std::vector<EventCallback> registeredEvents;
    std::mutex eventMutex;
public:

    void registerEvent(const EventCallback& EventCallback) {
        std::lock_guard lock(eventMutex);
        registeredEvents.push_back(EventCallback);
    }

    void unregisterEvent(const std::string& EventName) {
        std::lock_guard lock(eventMutex);
        if (registeredEvents.empty()) {
            return;
        }
        for (int i = 0; i < registeredEvents.size(); ++i) {
            if (registeredEvents.at(i).callbackName == EventName) {
                registeredEvents.erase(registeredEvents.begin() + i);
                break;
            }
        }
    }

    void triggerEvents() {
        std::lock_guard lock(eventMutex);
        for (const auto& event : registeredEvents) {
            event.func();
        }
    }

    void operator()() {
        triggerEvents();
    }

    void operator +=(const EventCallback& EventCallback) {
        registerEvent(EventCallback);
    }
};



#endif //EVENT_H
