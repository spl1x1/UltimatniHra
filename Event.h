//
// Created by Lukáš Kaplánek on 03.11.2025.
//

#ifndef SYNCEVENT_H
#define SYNCEVENT_H
#include <utility>

#include "MACROS.h"

#ifdef CLIENT
#include "render/Window.h"
#endif

#include "server/Server.h"

enum EventType {
    ServerStart,
    EntityMoved,
    EntityAdded,
    EntityRemoved,
    DataSync
};

enum EventSrc {
    ServerSrc,
    ClientSrc,
    RemoteSrc
};

struct Event {
    EventType type;
    EventSrc src;
    int recipientId;
};

class EventHandler {
    std::vector<void*> remoteClients;

    std::mutex mtx;

    std::queue<Event> eventQueue;
    std::queue<Event> priorityQueue;

public:
    explicit EventHandler(std::vector<void*> remoteClients = {}) {
        this->remoteClients = std::move(remoteClients);
    }
    void pushEvent(const Event& event, bool priority = false) {
        std::lock_guard<std::mutex> lock(mtx);
        if (priority) {
            priorityQueue.push(event);
        } else {
            eventQueue.push(event);
        }
    }

};




#endif //SYNCEVENT_H
